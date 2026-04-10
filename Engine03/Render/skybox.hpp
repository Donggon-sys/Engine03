//
//  skybox.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/4/10.
//

#pragma once
#include <simd/simd.h>
#include <vector>
#include <Metal/Metal.hpp>

struct Skybox {
    std::vector<simd::float4> position;
    std::vector<simd::float2> texCoord;
    std::vector<uint32_t> indices;
    MTL::Texture *image;
    MTL::RenderPipelineState *SkyboxPSO;
    Skybox();
    void draw(MTL::RenderCommandEncoder *encoder);
};
