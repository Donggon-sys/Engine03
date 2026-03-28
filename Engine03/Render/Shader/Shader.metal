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
};

[[vertex]] vertexOut vertexShader1(vertexIn in [[stage_in]],
                                   constant float4x4 &transfromMatrix [[buffer(10)]],
                                  constant float4x4 &viewProjectionMatrix [[buffer(11)]],
                                   constant float4x4 *jointMatrices [[buffer(12)]],
                                   
                                   constant bool& hasSkin [[buffer(13)]]) {
    float4 position = transfromMatrix * float4(in.pos, 1.0f);
    
//    if (hasSkin) {
//        position = in.weight0.x * (jointMatrices[in.joint0.x]) * position +
//                   in.weight0.y * (jointMatrices[in.joint0.y]) * position +
//                   in.weight0.z * (jointMatrices[in.joint0.z]) * position +
//                   in.weight0.w * (jointMatrices[in.joint0.w]) * position;
//    }
    
    if (hasSkin != 0 && jointMatrices != nullptr) {
        float4 skinnedPos = float4(0.0);
        skinnedPos += in.weight0.x * jointMatrices[in.joint0.x] * position;
        skinnedPos += in.weight0.y * jointMatrices[in.joint0.y] * position;
        skinnedPos += in.weight0.z * jointMatrices[in.joint0.z] * position;
        skinnedPos += in.weight0.w * jointMatrices[in.joint0.w] * position;
        
        // 可选：强制归一化（如果输入数据有问题）
        // float totalWeight = in.weight0.x + in.weight0.y + in.weight0.z + in.weight0.w;
        // if (totalWeight > 0.0) skinnedPos /= totalWeight;
        
        position = skinnedPos;
    }
    
    vertexOut out;
    out.vertexPosition = viewProjectionMatrix * position;
    out.texCoord = in.uv0;
    return out;
}

[[fragment]] float4 fragmentShader1(vertexOut in [[stage_in]],
                                   texture2d<float> texture[[texture(1)]]) {
    constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
    float4 out = texture.sample(textureSampler, in.texCoord);
//    out.z = 1.0f;
    return out;
//    float4 out = float4(1.4, 1.4, 1.4, 1.0);
//    return out;
}

