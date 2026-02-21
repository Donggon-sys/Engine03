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

struct vertexOut{
    float4 vertexPosition [[position]];
    float2 texCoord;
};

[[vertex]] vertexOut vertexShader1(vertexIn in [[stage_in]],
                                  constant float4x4& viewProjectionMatrix [[buffer(11)]]) {
    vertexOut out;
    
    out.vertexPosition = viewProjectionMatrix * float4(in.pos, 1.0f);
    out.texCoord = in.uv0;
    return out;
}

[[fragment]] float4 fragmentShader1(vertexOut in [[stage_in]],
                                   texture2d<float> texture[[texture(1)]]) {
    constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
    float4 out = texture.sample(textureSampler, in.texCoord);
//    out.z = 1.0f;
    return out;
}

