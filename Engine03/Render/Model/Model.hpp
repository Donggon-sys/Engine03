//
//  Model.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#pragma once
#include <vector>
#include <limits>
#include <string>
#include <simd/simd.h>
#include <Metal/Metal.hpp>
#include <tinygltf/tiny_gltf.h>

namespace mtlgltf {
constexpr uint32_t MAX_NUM_JOINTS = 128;

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
    uint32_t width, height;
    uint32_t mipLevels;
    uint32_t layerCount;
    MTL::PixelFormat pixelFormat;
    MTL::SamplerState *sampler;
    void updateDescriptor();
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
    Texture *baseColorTexture;
    Texture *metallicRoughnessTexture;
    Texture *normalTexture;
    Texture *occlusionTexture;
    Texture *emissiveTexture;
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
        Texture *specularGlossinessTexture;
        Texture *diffuseTexture;
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
    simd::float4x4 jointMatrix[MAX_NUM_JOINTS] { };
    uint32_t jointCount { 0 };
    uint32_t index;
    Mesh(simd::float4x4 matrix);
    ~Mesh();
    void setBoundingBox(simd::float3 min, simd::float3 max);
};

struct Skin {
    std::string name;
    Node *skeletonRoot = nullptr;
    std::vector<simd::float4x4> inverseBindMatrices;
    std::vector<Node *> joints;
};

struct Node {
    Node *parent;
    uint32_t index;
    std::vector<Node *> children;
    simd::float4x4 matrix;
    std::string name;
    Mesh *mesh;
    Skin *skin;
    int32_t skinIndex = -1;
    simd::float3 translation = simd::make_float3(0.0f, 0.0f, 0.0f);
    simd::float3 scale = simd::make_float3(1.0f, 1.0f, 1.0f);
    simd::quatf rotation = simd::quatf(0.0f, 0.0f, 0.0f, 1.0f);
    BoundingBox bb, aabb;
    bool useCacheMatrix { false };
    simd::float4x4 cachedLocalMatrix{ simd::float4x4(1.0f) };
    simd::float4x4 cachedMatrix{ simd::float4x4(1.0f) };
    simd::float4x4 localMatrix();
    simd::float4x4 getMatrix();
    void update();
    ~Node();
};

struct AnimationChannel {
    enum PathType { TRANSLATION, ROTATION, SCALE };
    PathType path;
    Node *node;
    uint32_t samplerIndex;
};

struct AnimationSampler {
    enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
    InterpolationType interpolation;
    std::vector<float> inputs;
    std::vector<simd::float4> outputsVec4;
    std::vector<float> outputs;
    simd::float4 cubicSplineInterpolation(size_t index, float time, uint32_t stride);
    void translate(size_t index, float time, Node *node);
    void scale(size_t index, float time, Node *node);
    void rotate(size_t index, float time, Node *node);
};

struct Animation {
    std::string name;
    std::vector<AnimationSampler> samplers;
    std::vector<AnimationChannel> channels;
    float start = std::numeric_limits<float>::max();
    float end = std::numeric_limits<float>::lowest();
};

class Model {
private:
    struct Vertex {
        simd::float3 position;
        simd::float3 normal;
        simd::float2 uv0;
        simd::float2 uv1;
        simd::uint4 joint0;
        simd::float4 weight0;
        simd::float4 color;
    };
    simd::float4 aabb;
    std::vector<Node *> nodes;
    std::vector<Node *> linearNodes;
    std::vector<Skin *> skins;
    std::vector<Texture *> textures;
    std::vector<TextureSampler *> textureSamplers;
    std::vector<Material *> materials;
    std::vector<Animation *> animations;
    std::vector<std::string> extensions;
    
    struct Dimensions {
        simd::float3 min = simd::make_float3(FLT_MAX, FLT_MAX, FLT_MAX);
        simd::float3 max = simd::make_float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    };
    MTL::Buffer *pVerticesBuffer = nullptr;
    MTL::Buffer *pIndicesBuffer = nullptr;
    
    void loadNode(Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale);
    void getNodeProps(const tinygltf::Node &node, const tinygltf::Model &model, size_t &vertexCount, size_t &indexCount);
    void loadSkin(tinygltf::Model &model);
    void drawNode(Node *node, MTL::RenderCommandEncoder *pEncoder);
    void loadTexture(tinygltf::Model &model, MTL::Device *device);
    void loadTextureSamplers(tinygltf::Model &model);
    void loadMaterials(tinygltf::Model &model);
    void loadAnimation(tinygltf::Model &model);
    void calculateBoundBox(Node *node, Node *parent);
    void getSceneDimensions();
    void updateAnimation(uint32_t index, float time);
    Node* fineNode(Node *parent, uint32_t index);
    Node* nodeFromIndex(uint32_t index);
    
public:
    Model();
    ~Model();
    Model(const Model &other) = delete;
    Model& operator=(Model &other) = delete;
    Model(Model &&other);
    Model& operator=(Model &&other);
    
    void loadModel(MTL::Device *device, std::string fileName);
    void draw(MTL::RenderCommandEncoder *pEncoder);
};
}

