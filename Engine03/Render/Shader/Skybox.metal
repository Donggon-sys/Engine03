//
//  Skybox.metal
//  Engine03
//
//  Created by Chenruyi on 2026/4/12.
//

#include <metal_stdlib>
using namespace metal;


struct vertexIn {
    float3 pos [[attribute(0)]];
    float3 normal [[attribute(1)]];
    float2 uv0 [[attribute(2)]];
};

struct vertexOut {
    float4 vertexPosition [[position]];
    float2 texCoord;
    float4 color;
};

[[vertex]] vertexOut skyboxVertexShader(vertexIn in [[stage_in]],
                                   constant float4x4 &transfromMatrix [[buffer(10)]],
                                  constant float4x4 &viewProjectionMatrix [[buffer(11)]],
                                   constant float4x4 *jointMatrices [[buffer(12)]]
                                   ) {
    float4 position = transfromMatrix * float4(in.pos, 1.0f);
    
    vertexOut out;
    out.vertexPosition = viewProjectionMatrix * position;
    out.texCoord = in.uv0;
    return out;
}

[[fragment]] float4 skyboxFragmentShader(vertexOut in [[stage_in]],
                                   texture2d<float> texture[[texture(1)]],
                                         sampler textureSampler[[sampler(1)]]) {
    float4 out = texture.sample(textureSampler, in.texCoord);
    return out;
}
