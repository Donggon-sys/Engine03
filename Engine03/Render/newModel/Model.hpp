//
//  Model.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/3/31.
//

#pragma once
#include <string>
#include <vector>
#include <Metal/Metal.hpp>
#include <simd/simd.h>

namespace BTflag {
namespace Model {

struct TextureSampler {
    MTL::SamplerMinMagFilter magFilter;
    MTL::SamplerMinMagFilter minFilter;
    MTL::SamplerAddressMode addressModeU;
    MTL::SamplerAddressMode addressModeV;
    MTL::SamplerAddressMode addressModeW;
};

struct Material {
    enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
    AlphaMode alphaMode = ALPHAMODE_OPAQUE;
    float alphaCutoff = 1.0f;
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    simd::float4 baseColorFactor = simd::make_float4(1.0f, 1.0f, 1.0f, 1.0f);
    simd::float4 emissiveFactor = simd::make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    MTL::Texture *pTexture0 = nullptr;
    MTL::Texture *pTexture1 = nullptr;
    bool doubleSided = false;
    
    struct PBRWorkflows {
        bool metallicRoughness = true;
        bool specularGlossiness = false;
    } pbrWorkflows;
    bool unlit = false;
    float emissiveStrength = 1.0f;
};

struct Mesh {
    MTL::Buffer *pPositionBuffer = nullptr;
    MTL::Buffer *pNormalBuffer = nullptr;
    MTL::Buffer *pTexCoord0Buffer = nullptr;
    MTL::Buffer *pTexCoord1Buffer = nullptr;
    MTL::Buffer *pJointsBuffer = nullptr;
    MTL::Buffer *pWeightsBuffer = nullptr;
    MTL::Buffer *pColorBuffer = nullptr;
    MTL::Buffer *pIndicesBuffer = nullptr;
};

struct Joint {
    simd::quatf rotation;
    simd::float4 translate;
    simd::float3 scale;
};

struct Skin {
    std::string name;
    MTL::Buffer *pJointMatrices = nullptr;
    std::vector<simd::float4x4> inverseBindMatrices;
};

struct AnimationChannel {
    enum PathType { TRANSLATION, ROTATION, SCALE };
    PathType path;
};

struct Animation {
    std::string name;
//    std::vector<AnimationSampler> samplers;
    std::vector<AnimationChannel> channels;
    float start = std::numeric_limits<float>::max();
    float end = std::numeric_limits<float>::min();
};

class Model {
    MTL::Device *pDevice;

    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::vector<Skin> skins;
    
    Model(std::string fileName);
};
}
}
