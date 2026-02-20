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
#include "Camera.hpp"

class Render {
private:
    Scene *pScene;
    MTL::Device *pDevice;
    MTL::Library *pLibrary;
    MTL::CommandQueue *pCommandQueue;
    MTL::CommandBuffer *pCommandBuffer;
    MTL::Texture *pDepthTexture;
    simd::uint2 viewPortSize;
    MTL::RenderPassDescriptor *createRenderPassDescriptor(CA::MetalDrawable *drawable);
    void createDepthtexture();
    
public:
    Render(CA::MetalLayer *layer);
    ~Render();
    void draw(CA::MetalLayer *layer);
    void changeSize(int *width, int *height);
    void goForward();
    void goBack();
    void moveLeft();
    void moveRight();
    void mouse(float deltaX, float deltaY);
};
