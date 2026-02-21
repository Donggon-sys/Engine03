//
//  Scene.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#include "Scene.hpp"

Scene::Scene() {
    pCamera = new Camera();
}

Scene::~Scene() {
    modelList.clear();
    PSOList.clear();
    delete pCamera;
}

void Scene::createScene(MTL::Device *device, MTL::Library *library) {
    createPipelineState(device, library);
    createDepthStencilState(device);
    
    SModel m2 = SModel(device);
    m2.openFile("ground.glb");
    smodelList.push_back(std::move(m2));
    
    mtlgltf::Model mod1 = mtlgltf::Model();
    mod1.loadModel(device, "ball01.glb", device->newCommandQueue(), 1.0f);
    modelList.push_back(std::move(mod1));
}

void Scene::renderScene(MTL::RenderCommandEncoder *encoder) {
    viewProjectionMatrix = pCamera->getViewProjectionMatrix();
    for (SModel &model : smodelList) {
        encoder->setRenderPipelineState(PSOList.at(MaterialType::DEFAULT));
        encoder->setDepthStencilState(depthStencilState);
        encoder->setVertexBytes(&viewProjectionMatrix, sizeof(viewProjectionMatrix), NS::UInteger(viewProjectionBufferIndex));
        model.renderModel(encoder);
    }
    for (mtlgltf::Model &model : modelList) {
        encoder->setRenderPipelineState(PSOList.at(MaterialType::SPECIAL));
        encoder->setDepthStencilState(depthStencilState);
        encoder->setVertexBytes(&viewProjectionMatrix, sizeof(viewProjectionMatrix), NS::UInteger(11));
        model.draw(encoder);
    }
}

void Scene::setViewProjectionMatrix(simd::float4x4 matrix) {
    viewProjectionMatrix = matrix;
}

void Scene::createDepthStencilState(MTL::Device *device) {
    MTL::DepthStencilDescriptor *descriptor = MTL::DepthStencilDescriptor::alloc()->init();
    descriptor->setDepthCompareFunction(MTL::CompareFunctionLess);
    descriptor->setDepthWriteEnabled(true);
    depthStencilState = device->newDepthStencilState(descriptor);
    descriptor->release();
}

void Scene::createPipelineState(MTL::Device *device, MTL::Library *library) {
    // TODO: 创建默认PSO
    {
        MTL::Function *vertexShader = library->newFunction(NS::String::string("vertexShader", NS::UTF8StringEncoding));
        MTL::Function *fragmentShader = library->newFunction(NS::String::string("fragmentShader", NS::UTF8StringEncoding));
        
        MTL::RenderPipelineDescriptor *descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        descriptor->setLabel(NS::String::string("model Rendering Pipeline", NS::UTF8StringEncoding));
        descriptor->setVertexFunction(vertexShader);
        descriptor->setFragmentFunction(fragmentShader);
        descriptor->colorAttachments()->object(NS::UInteger(0))->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        
        MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
        vertexDescriptor->attributes()->object(NS::UInteger(0))->setFormat(MTL::VertexFormatFloat3);
        vertexDescriptor->attributes()->object(NS::UInteger(0))->setOffset(NS::UInteger(0));
        vertexDescriptor->attributes()->object(NS::UInteger(0))->setBufferIndex(NS::UInteger(vertexPositionBufferIndex));
        vertexDescriptor->layouts()->object(NS::UInteger(0))->setStride(NS::UInteger(sizeof(simd::float3)));
        
        vertexDescriptor->attributes()->object(NS::UInteger(1))->setFormat(MTL::VertexFormatFloat2);
        vertexDescriptor->attributes()->object(NS::UInteger(1))->setOffset(NS::UInteger(0));
        vertexDescriptor->attributes()->object(NS::UInteger(1))->setBufferIndex(NS::UInteger(vertexTexCoordBufferIndex));
        vertexDescriptor->layouts()->object(NS::UInteger(1))->setStride(NS::UInteger(sizeof(simd::float2)));
        
        descriptor->setVertexDescriptor(vertexDescriptor);
        descriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);

        NS::Error *error;
        MTL::RenderPipelineState *PSO = device->newRenderPipelineState(descriptor, &error);
        
        vertexDescriptor->release();
        descriptor->release();
        vertexShader->release();
        fragmentShader->release();
        
        PSOList.insert({MaterialType::DEFAULT, PSO});
    }
    
    // TODO: 创建特殊PSO
    {
        MTL::Function *vertexShader = library->newFunction(NS::String::string("vertexShader1", NS::UTF8StringEncoding));
        MTL::Function *fragmentShader = library->newFunction(NS::String::string("fragmentShader1", NS::UTF8StringEncoding));
        
        MTL::RenderPipelineDescriptor *descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        descriptor->setLabel(NS::String::string("model Rendering Pipeline", NS::UTF8StringEncoding));
        descriptor->setVertexFunction(vertexShader);
        descriptor->setFragmentFunction(fragmentShader);
        descriptor->colorAttachments()->object(NS::UInteger(0))->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        
        {
            MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
            vertexDescriptor->attributes()->object(NS::UInteger(0))->setFormat(MTL::VertexFormat::VertexFormatFloat3);
            vertexDescriptor->attributes()->object(NS::UInteger(0))->setOffset(NS::UInteger(0));
            vertexDescriptor->attributes()->object(NS::UInteger(0))->setBufferIndex(NS::UInteger(0));
            vertexDescriptor->layouts()->object(NS::UInteger(0))->setStride(NS::UInteger(sizeof(simd::float3)));

            vertexDescriptor->attributes()->object(NS::UInteger(1))->setFormat(MTL::VertexFormat::VertexFormatFloat3);
            vertexDescriptor->attributes()->object(NS::UInteger(1))->setOffset(NS::UInteger(0));
            vertexDescriptor->attributes()->object(NS::UInteger(1))->setBufferIndex(NS::UInteger(1));
            vertexDescriptor->layouts()->object(NS::UInteger(1))->setStride(NS::UInteger(sizeof(simd::float3)));
            
            vertexDescriptor->attributes()->object(NS::UInteger(2))->setFormat(MTL::VertexFormat::VertexFormatFloat2);
            vertexDescriptor->attributes()->object(NS::UInteger(2))->setOffset(NS::UInteger(0));
            vertexDescriptor->attributes()->object(NS::UInteger(2))->setBufferIndex(NS::UInteger(2));
            vertexDescriptor->layouts()->object(NS::UInteger(2))->setStride(NS::UInteger(sizeof(simd::float2)));
            
            vertexDescriptor->attributes()->object(NS::UInteger(3))->setFormat(MTL::VertexFormat::VertexFormatFloat2);
            vertexDescriptor->attributes()->object(NS::UInteger(3))->setOffset(NS::UInteger(0));
            vertexDescriptor->attributes()->object(NS::UInteger(3))->setBufferIndex(NS::UInteger(3));
            vertexDescriptor->layouts()->object(NS::UInteger(3))->setStride(NS::UInteger(sizeof(simd::float2)));
            
            vertexDescriptor->attributes()->object(NS::UInteger(4))->setFormat(MTL::VertexFormat::VertexFormatUInt4);
            vertexDescriptor->attributes()->object(NS::UInteger(4))->setOffset(NS::UInteger(0));
            vertexDescriptor->attributes()->object(NS::UInteger(4))->setBufferIndex(NS::UInteger(4));
            vertexDescriptor->layouts()->object(NS::UInteger(4))->setStride(NS::UInteger(sizeof(simd::uint4)));
            
            vertexDescriptor->attributes()->object(NS::UInteger(5))->setFormat(MTL::VertexFormat::VertexFormatFloat4);
            vertexDescriptor->attributes()->object(NS::UInteger(5))->setOffset(NS::UInteger(0));
            vertexDescriptor->attributes()->object(NS::UInteger(5))->setBufferIndex(NS::UInteger(5));
            vertexDescriptor->layouts()->object(NS::UInteger(5))->setStride(NS::UInteger(sizeof(simd::float4)));
            
            vertexDescriptor->attributes()->object(NS::UInteger(6))->setFormat(MTL::VertexFormat::VertexFormatFloat4);
            vertexDescriptor->attributes()->object(NS::UInteger(6))->setOffset(NS::UInteger(0));
            vertexDescriptor->attributes()->object(NS::UInteger(6))->setBufferIndex(NS::UInteger(6));
            vertexDescriptor->layouts()->object(NS::UInteger(6))->setStride(NS::UInteger(sizeof(simd::float4)));
            
            descriptor->setVertexDescriptor(vertexDescriptor);
            vertexDescriptor->release();
        }
        
        descriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);

        NS::Error *error;
        MTL::RenderPipelineState *PSO = device->newRenderPipelineState(descriptor, &error);
        
        descriptor->release();
        vertexShader->release();
        fragmentShader->release();
        
        PSOList.insert({MaterialType::SPECIAL, PSO});
    }
}

void Scene::setViewPortSize(simd::uint2 v) {
    pCamera->setAspect(static_cast<float>(v.x) / static_cast<float>(v.y));
}

void Scene::goForward() {
    pCamera->goForward();
}

void Scene::goBack() {
    pCamera->goBack();
}

void Scene::moveLeft() {
    pCamera->moveLeft();
}

void Scene::moveRight() {
    pCamera->moveRight();
}

void Scene::setMouse(float deltaX, float deltaY) {
    pCamera->mouse(deltaX, deltaY);
}
