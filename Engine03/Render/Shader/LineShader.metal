//
//  LineShader.metal
//  Engine03
//
//  Created by Chenruyi on 2026/4/2.
//

#include <metal_stdlib>
using namespace metal;

// LineShader.metal
#include <metal_stdlib>
using namespace metal;

struct LineVertex {
    float3 pos;
    float4 color;
};

struct LineOut {
    float4 pos [[position]];
    float4 color;
};

// 不用 [[stage_in]]，直接读取 buffer
[[vertex]] LineOut debugLineVert(const device LineVertex *vertices [[buffer(0)]],
                                   uint vid [[vertex_id]],
                                   constant float4x4 &viewProjectionMatrix [[buffer(1)]]) {
    LineOut out;
    out.pos = viewProjectionMatrix * float4(vertices[vid].pos, 1.0);
    out.color = vertices[vid].color;
    return out;
}

[[fragment]] float4 debugLineFrag(LineOut in [[stage_in]]) {
    return in.color;
}
