//
//  FragmentShader.metal
//  Engine03
//
//  Created by Chenruyi on 2026/2/11.
//

#include <metal_stdlib>

#include "ShaderType.h"

using namespace metal;

[[fragment]] float4 fragmentShader(vertexOut in [[stage_in]],
                                   texture2d<float> texture[[texture(1)]]) {
    constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
    float4 out = texture.sample(textureSampler, in.texCoord);
//    out.z = 1.0f;
    return out;
}


