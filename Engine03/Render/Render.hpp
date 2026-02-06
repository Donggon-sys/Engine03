//
//  Render.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#pragma once
#include <QuartzCore/CAMetalLayer.hpp>
#include <Metal/Metal.hpp>
#include <simd/simd.h>

#include "Scene.hpp"

class Render {
private:
    MTL::Device *pDevice;
    MTL::Library *pLibrary;
    MTL::CommandQueue *pCommandQueue;
    MTL::CommandBuffer *pCommandBuffer;
    simd::uint2 viewPortSize;
    
    Scene *pScene;
    
public:
    Render(CA::MetalLayer *layer);
    ~Render();
    void draw(CA::MetalLayer *layer);
    void changeSize(int *width, int *height);
};
