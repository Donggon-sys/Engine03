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
    float4 tangent [[attribute(7)]];
};

struct vertexOut {
    float4 vertexPosition [[position]];
    float2 uv0;
    float2 uv1;
    float4 color;
    float3 B, T, N;
};

[[vertex]] vertexOut vertexShader1(vertexIn in [[stage_in]],
                                   constant float4x4 &transfromMatrix [[buffer(10)]],
                                  constant float4x4 &viewProjectionMatrix [[buffer(11)]],
                                   constant float4x4 *jointMatrices [[buffer(12)]],
                                   
                                   constant bool& hasSkin [[buffer(13)]]) {
    float4 position = transfromMatrix * float4(in.pos, 1.0f);
    float3 N = in.normal;
    float3 T = in.tangent.xyz;
    
    if (hasSkin != 0 && jointMatrices != nullptr) {
        float4x4 skinMatrix =  in.weight0.x * jointMatrices[in.joint0.x]
                             + in.weight0.y * jointMatrices[in.joint0.y]
                             + in.weight0.z * jointMatrices[in.joint0.z]
                             + in.weight0.w * jointMatrices[in.joint0.w];
        position = skinMatrix * position;
        
        // 创建normal matrix
        float3x3 normalMatrix = float3x3(skinMatrix[0].xyz, skinMatrix[1].xyz, skinMatrix[2].xyz);
        
        N = normalize(normalMatrix * N);
        T = normalize(normalMatrix * T);
        T = normalize(T - N * dot(N, T));
    }
    float3 B = cross(N, T) * in.tangent.w;
    
    vertexOut out;
    out.vertexPosition = viewProjectionMatrix * position;
    out.uv0 = in.uv0;
    out.uv1 = in.uv1;
    out.N = N;
    out.T = T;
    out.B = B;
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
                                   texture2d<float> baseColorTexture[[texture(0)]],
                                    sampler baseColorSampler [[sampler(0)]],
                                    texture2d<float> normalTexture[[texture(1)]],
                                    sampler normalSampler [[sampler(1)]]) {
//    constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
    float4 objectColor = baseColorTexture.sample(baseColorSampler, in.uv0);
    
    // 灯光的属性
    float3 lightColor = float3(1.0f, 1.0f, 1.0f);
//    float3 lightPos = float3(0.0f, 6.0f, 0.0f);
    float3 lightDir = normalize(float3(0.5f, 1.0f, 0.3f));
    
    
    float3x3 BTN = float3x3(in.T, in.B, in.N);
    
    
    float3 normal = normalize(normalTexture.sample(normalSampler, in.uv0).xyz * 2.0f - 1.0f);
    normal = BTN * normal;
    float NdotL = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = objectColor.xyz * lightColor * NdotL;
    
    return float4(diffuse, objectColor.w);
}
