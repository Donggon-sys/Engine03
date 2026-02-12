//
//  Render.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#include "Render.hpp"

Render::Render(CA::MetalLayer *layer) {
    viewPortSize = simd::make_uint2(800, 600);
    pDevice = layer->device();
    pLibrary = pDevice->newDefaultLibrary();
    pCommandQueue = pDevice->newCommandQueue();
    
    //TODO: 创建对应的buffer,以后是在scene中处理buffer的
    pScene = new Scene();
    pScene->createScene(pDevice, pLibrary);
    pCamera = new Camera();
}

Render::~Render() {
    delete pScene;
    delete pCamera;
}

MTL::RenderPassDescriptor *Render::createRenderPassDescriptor(CA::MetalDrawable *drawable) {
    MTL::RenderPassDescriptor *targetRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setTexture(drawable->texture());
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setLoadAction(MTL::LoadActionClear);
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setStoreAction(MTL::StoreActionStore);
    
    return targetRenderPassDescriptor;
}

void Render::draw(CA::MetalLayer *layer) {
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
    
    CA::MetalDrawable *_pDrawable = layer->nextDrawable();
    if (!_pDrawable) {
        pool->release();
        return;
    }
    layer->setDrawableSize(CGSizeMake(viewPortSize.x * 2, viewPortSize.y * 2));
    
    pCommandBuffer = pCommandQueue->commandBuffer();
    
    MTL::RenderPassDescriptor *_pTargetRenderPassDescriptor = createRenderPassDescriptor(_pDrawable);
    MTL::RenderCommandEncoder *_pEncoder = pCommandBuffer->renderCommandEncoder(_pTargetRenderPassDescriptor);
    
    //TODO: 使用encoder去编码命令,以后使用scene中的函数，提过传入encoder实现
    pCamera->setAspect(static_cast<float>(viewPortSize.x) / static_cast<float>(viewPortSize.y));
    pScene->setViewProjectionMatrix(pCamera->getViewProjectionMatrix());
    pScene->renderScene(_pEncoder);
    
    _pEncoder->endEncoding();
    pCommandBuffer->presentDrawable(_pDrawable);
    pCommandBuffer->commit();
    pCommandBuffer->waitUntilCompleted();
    
    _pTargetRenderPassDescriptor->release();
    
    pool->release();
}

void Render::changeSize(int *width, int *height) {
    uint Uwidth = (uint)*width;
    uint Uheight = (uint)*height;
    
    viewPortSize.x = Uwidth;
    viewPortSize.y = Uheight;
}

void Render::mouse(float delatX, float delatY) {
    pCamera->mouse(delatX, delatY);
}

void Render::goForward() {
    pCamera->goForward();
}

void Render::goBack() {
    pCamera->goBack();
}

void Render::moveLeft() {
    pCamera->moveLeft();
}

void Render::moveRight() {
    pCamera->moveRight();
}
