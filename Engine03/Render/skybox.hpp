//
//  skybox.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/4/10.
//

#pragma once
#include <simd/simd.h>
#include <vector>
#include <tinygltf/tiny_gltf.h>
#include <Metal/Metal.hpp>

namespace BTflag {
namespace Skybox {

struct Node;

struct BoundingBox {
    simd::float3 min;
    simd::float3 max;
    bool valid = false;
    BoundingBox();
    BoundingBox(simd::float3 min, simd::float3 max);
    BoundingBox getAABB(simd::float4x4 m);
};

struct TextureSampler {
    MTL::SamplerMinMagFilter magFilter;
    MTL::SamplerMinMagFilter minFilter;
    MTL::SamplerAddressMode addressModeU;
    MTL::SamplerAddressMode addressModeV;
    MTL::SamplerAddressMode addressModeW;
};

struct Texture {
    MTL::Texture *image;
    MTL::PixelFormat pixelFormat;
    MTL::SamplerState *sampler;
    void destroy();
    void fromgltfImage(tinygltf::Image &gltfimage, std::string path, TextureSampler textureSampler, MTL::Device *device, MTL::CommandQueue *queue);
};

struct Material {
    enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
    AlphaMode alphaMode = ALPHAMODE_OPAQUE;
    float alphaCutoff = 1.0f;
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    simd::float4 baseColorFactor = simd::make_float4(1.0f, 1.0f, 1.0f, 1.0f);
    simd::float4 emissiveFactor = simd::make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    Texture *baseColorTexture = nullptr;
    Texture *metallicRoughnessTexture = nullptr;
    Texture *normalTexture = nullptr;
    Texture *occlusionTexture = nullptr;
    Texture *emissiveTexture = nullptr;
    bool doubleSided = false;
    struct TexCoordSets {
        uint8_t baseColor = 0;
        uint8_t metallicRoughness = 0;
        uint8_t specularGlossiness = 0;
        uint8_t normal = 0;
        uint8_t occlusion = 0;
        uint8_t emissive = 0;
    } texCoordSets;
    struct Extension {
        Texture *specularGlossinessTexture = nullptr;
        Texture *diffuseTexture = nullptr;
        simd::float4 diffuseFactor = simd::make_float4(1.0f, 1.0f, 1.0f, 1.0f);
        simd::float3 specularFactor = simd::make_float3(0.0f, 0.0f, 0.0f);
    } extension;
    struct PBRWorkflows {
        bool metallicRoughness = true;
        bool specularGlossiness = false;
    } pbrWorkflows;
    int index = 0;
    bool unlit = false;
    float emissiveStrength = 1.0f;
};

struct Primitive {
    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t vertexCount;
    Material &material;
    bool hasIndices;
    BoundingBox bb;
    Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material &material);
    void setBoundingBox(simd::float3 min, simd::float3 max);
};

struct Mesh {
    std::vector<Primitive *> primitives;
    BoundingBox bb, aabb;
    simd::float4x4 matrix;
    uint32_t jointCount { 0 };
    uint32_t index;
    Mesh(simd::float4x4 matrix);
    ~Mesh();
    void setBoundingBox(simd::float3 min, simd::float3 max);
};

struct Node {
    Node *parent;
    uint32_t index;
    std::vector<Node *> children;
    simd::float4x4 matrix;
    std::string name;
    Mesh *mesh;
    simd::float3 translation = simd::make_float3(0.0f, 0.0f, 0.0f);
    simd::float3 scale = simd::make_float3(1.0f, 1.0f, 1.0f);
    simd::quatf rotation = simd::quatf(0.0f, 0.0f, 0.0f, 1.0f);
    BoundingBox bvh, aabb;
    bool useCacheMatrix { false };
    simd::float4x4 cachedLocalMatrix{ simd::float4x4(1.0f) };
    simd::float4x4 cachedMatrix{ simd::float4x4(1.0f) };
    simd::float4x4 localMatrix();
    simd::float4x4 getMatrix();
    void update();
    ~Node();
};

struct LoaderInfo {
    size_t indexPos = 0;
    size_t vertexPos = 0;
};

struct Model {
    std::vector<simd::float3> position;
    std::vector<simd::float3> normal;
    std::vector<simd::float2> uv0;
    std::vector<unsigned int> vertexIndices;
    
    MTL::Buffer *pPositionBuffer = nullptr;
    MTL::Buffer *pNormalBuffer = nullptr;
    MTL::Buffer *pTexCoord0Buffer = nullptr;
    MTL::Buffer *pIndicesBuffer = nullptr;
    
    MTL::Device *pDevice;
    simd::float4x4 aabb;
    std::vector<Node *> nodes;
    std::vector<Node *> linearNodes;
    std::vector<Texture> textures;
    std::vector<TextureSampler> textureSamplers;
    std::vector<Material> materials;
    std::vector<std::string> extensions;
    
    struct Dimensions {
        simd::float3 min = simd::make_float3(FLT_MAX, FLT_MAX, FLT_MAX);
        simd::float3 max = simd::make_float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    } dimensions;
    
    void loadNode(Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, LoaderInfo &loaderInfo, float globalscale);
    void loadSkin(tinygltf::Model &model);
    void drawNode(Node *node, MTL::RenderCommandEncoder *pEncoder);
    void loadTexture(tinygltf::Model &model, MTL::Device *device, MTL::CommandQueue *queue);
    void loadTextureSamplers(tinygltf::Model &model);
    void loadMaterials(tinygltf::Model &model);
    void calculateBoundBox(Node *node, Node *parent);
    void getSceneDimensions();
    void clearup();
    Node* fineNode(Node *parent, uint32_t index);
    Node* nodeFromIndex(uint32_t index);
    MTL::SamplerMinMagFilter getFilterMode(int32_t filterMode);
    MTL::SamplerAddressMode getWarpMode(int32_t warpMode);
    Model();
    ~Model();
    Model(const Model &other) = delete;
    Model& operator=(Model &other) = delete;
    Model(Model &&other);
    Model& operator=(Model &&other);

    void loadModel(MTL::Device *device, std::string fileName, MTL::CommandQueue *queue, float scale);
    void draw(MTL::RenderCommandEncoder *pEncoder, MTL::RenderPipelineState* pipelineState, MTL::DepthStencilState* depthStencilState);
};
};
}
