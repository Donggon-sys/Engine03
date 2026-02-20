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
    
    pDepthTexture = nullptr;
    createDepthtexture();
}

Render::~Render() {
    delete pScene;
    pDepthTexture->release();
    pLibrary->release();
    pCommandQueue->release();
}

void Render::createDepthtexture() {
    if (pDepthTexture) {
        pDepthTexture->release();
        pDepthTexture = nullptr;
    }
    
    MTL::TextureDescriptor *textureDescriptor = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatDepth32Float, NS::UInteger(viewPortSize.x * 2), NS::UInteger(viewPortSize.y * 2), false);
    textureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
    textureDescriptor->setStorageMode(MTL::StorageModePrivate);
    pDepthTexture = pDevice->newTexture(textureDescriptor);
    textureDescriptor->release();
}

MTL::RenderPassDescriptor *Render::createRenderPassDescriptor(CA::MetalDrawable *drawable) {
    MTL::RenderPassDescriptor *targetRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setTexture(drawable->texture());
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setLoadAction(MTL::LoadActionClear);
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    targetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0))->setStoreAction(MTL::StoreActionStore);
    
    targetRenderPassDescriptor->depthAttachment()->setTexture(pDepthTexture);
    targetRenderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
    targetRenderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);
    targetRenderPassDescriptor->depthAttachment()->setClearDepth(1.0);
    
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
    pScene->setViewPortSize(viewPortSize);
    _pEncoder->setCullMode(MTL::CullModeBack);
//    _pEncoder->setFrontFacingWinding(MTL::WindingClockwise);
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
    
    createDepthtexture();
}

void Render::mouse(float delatX, float delatY) {
    pScene->setMouse(delatX, delatY);
}

void Render::goForward() {
    pScene->goForward();
}

void Render::goBack() {
    pScene->goBack();
}

void Render::moveLeft() {
    pScene->moveLeft();
}

void Render::moveRight() {
    pScene->moveRight();
}
