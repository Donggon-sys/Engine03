//
//  FragmentShader.metal
//  Engine03
//
//  Created by Chenruyi on 2026/2/11.
//

#include <metal_stdlib>

#include "ShaderType.h"

using namespace metal;

[[fragment]] float4 fragmentShader(vertexOut in [[stage_in]]) {
    return float4(182.0f/255.0f, 240.0f/255.0f, 228.0f/255.0f, 1.0f);
}


