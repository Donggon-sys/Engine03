//
//  Shader.metal
//  Engine03
//
//  Created by Chenruyi on 2026/2/21.
//

#include <metal_stdlib>
using namespace metal;

struct vertexIn {
    float3 pos [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 uv0 [[attribute(2)]];
    float2 uv1 [[attribute(3)]];
    uint4 joint0 [[attribute(4)]];
    float4 weight0 [[attribute(5)]];
    float4 color [[attribute(6)]];
};

struct vertexOut {
    float4 vertexPosition [[position]];
    float2 texCoord;
    float4 color;
};

[[vertex]] vertexOut vertexShader1(vertexIn in [[stage_in]],
                                   constant float4x4 &transfromMatrix [[buffer(10)]],
                                  constant float4x4 &viewProjectionMatrix [[buffer(11)]],
                                   constant float4x4 *jointMatrices [[buffer(12)]],
                                   
                                   constant bool& hasSkin [[buffer(13)]]) {
    float4 position = transfromMatrix * float4(in.pos, 1.0f);
    
    if (hasSkin != 0 && jointMatrices != nullptr) {
        float4 skinnedPos = float4(0.0);
        skinnedPos += in.weight0.x * jointMatrices[in.joint0.x] * position;
        skinnedPos += in.weight0.y * jointMatrices[in.joint0.y] * position;
        skinnedPos += in.weight0.z * jointMatrices[in.joint0.z] * position;
        skinnedPos += in.weight0.w * jointMatrices[in.joint0.w] * position;
        position = skinnedPos;
    }
    
    vertexOut out;
    out.vertexPosition = viewProjectionMatrix * position;
    out.texCoord = in.uv0;
    return out;
}

struct Material {
    float4 baseColorFactor [[id(8)]];
    float metallicFactor [[id(5)]];
    float roughnessFactor [[id(6)]];
    float4 emissiveFactor [[id(9)]];
    float emissiveStrength  [[id(11)]];
    
    texture2d<float> baseColorTex [[id(0)]];
    texture2d<float> normalTex [[id(1)]];
    texture2d<float> metallicRoughnessTex [[id(2)]];
    texture2d<float> occlusionTex [[id(3)]];
    texture2d<float> emissiveTex [[id(4)]];

};

[[fragment]] float4 fragmentShader1(vertexOut in [[stage_in]],
                                   texture2d<float> texture[[texture(1)]]) {
    constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
    float4 out = texture.sample(textureSampler, in.texCoord);
    return out;
}
