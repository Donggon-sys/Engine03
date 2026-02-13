//
//  ShaderType.h
//  Engine03
//
//  Created by Chenruyi on 2026/2/11.
//

#pragma once

#include <simd/simd.h>

typedef enum bufferIndex{
    vertexPositionBufferIndex = 0,
    vertexTexCoordBufferIndex = 1,
    vertexNormalBufferIndex   = 2,
    viewProjectionBufferIndex = 11,
} bufferIndex;

typedef struct vertexIn{
    simd::float3 vertexPosition [[attribute(vertexPositionBufferIndex)]];
    simd::float2 texCoord[[attribute(vertexTexCoordBufferIndex)]];
}vertexIn;

typedef struct vertexOut{
    simd::float4 vertexPosition [[position]];
    simd::float2 texCoord;
}vertexOut;
