//
//  skybox.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/4/10.
//

#include "skybox.hpp"
#include <tinygltf/stb_image.h>

Skybox::Skybox() {
    position = {
        simd::make_float4(-270.0f, -270.0f, -270.0f, 1.0f),
        simd::make_float4( 270.0f, -270.0f, -270.0f, 1.0f),
        simd::make_float4( 270.0f,  270.0f, -270.0f, 1.0f),
        simd::make_float4(-270.0f,  270.0f, -270.0f, 1.0f),
        simd::make_float4(-270.0f, -270.0f,  270.0f, 1.0f),
        simd::make_float4( 270.0f, -270.0f,  270.0f, 1.0f),
        simd::make_float4( 270.0f,  270.0f,  270.0f, 1.0f),
        simd::make_float4(-270.0f,  270.0f,  270.0f, 1.0f),
    };
}

void Skybox::draw(MTL::RenderCommandEncoder *encoder) {
    
}
