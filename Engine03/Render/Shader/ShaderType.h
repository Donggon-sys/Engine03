//
//  ShaderType.h
//  Engine03
//
//  Created by Chenruyi on 2026/2/11.
//

#pragma once

#include <simd/simd.h>

typedef enum bufferIndex{
    vertexPositionBuffer = 0,
    viewProjectionBuffer = 11,
} bufferIndex;

typedef struct vertexIn{
    simd::float3 vertexPosition [[attribute(0)]];
}vertexIn;

typedef struct vertexOut{
    simd::float4 vertexPosition [[position]];
}vertexOut;
