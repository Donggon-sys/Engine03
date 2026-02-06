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
}

Render::~Render() {
    delete pScene;
}

void Render::draw(CA::MetalLayer *layer) {
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
    
    CA::MetalDrawable *_pDrawable = layer->nextDrawable();
    if (!_pDrawable) {
        pool->release();
        return;
    }
    layer->setDrawableSize(CGSizeMake(viewPortSize.x * 2, viewPortSize.y * 2));
    MTL::RenderPassDescriptor *_pTargetRenderPassDescriptor;
    _pTargetRenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
    
    MTL::RenderPassColorAttachmentDescriptor *cd = _pTargetRenderPassDescriptor->colorAttachments()->object(NS::UInteger(0));
    cd->setTexture(_pDrawable->texture());
    cd->setLoadAction(MTL::LoadActionClear);
    cd->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    cd->setStoreAction(MTL::StoreActionStore);
    
    pCommandBuffer = pCommandQueue->commandBuffer();
    MTL::RenderCommandEncoder *_pEncoder = pCommandBuffer->renderCommandEncoder(_pTargetRenderPassDescriptor);
    
    //TODO: 使用encoder去编码命令,以后使用scene中的函数，提过传入encoder实现
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
