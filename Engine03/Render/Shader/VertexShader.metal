//
//  VertexShader.metal
//  Engine03
//
//  Created by Chenruyi on 2026/2/11.
//

#include <metal_stdlib>

#include "ShaderType.h"

using namespace metal;

[[vertex]] vertexOut vertexShader(vertexIn in [[stage_in]],
                                  constant float4x4& viewProjectionMatrix [[buffer(viewProjectionBuffer)]]
                                  ) {
    vertexOut out;
    
    out.vertexPosition = viewProjectionMatrix * float4(in.vertexPosition, 1.0f);
    return out;
}
