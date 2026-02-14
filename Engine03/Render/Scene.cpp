//
//  Scene.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#include "Scene.hpp"

Scene::Scene() {

}

Scene::~Scene() {
    modelList.clear();
}

void Scene::createScene(MTL::Device *device, MTL::Library *library) {
    createPipelineState(device, library);
    createDepthStencilState(device);
    
    Model m1 = Model(device);
    m1.openFile("ball01.glb");
    modelList.push_back(std::move(m1));
    
    Model m2 = Model(device);
    m2.openFile("ground01.glb");
    modelList.push_back(std::move(m2));
}

void Scene::renderScene(MTL::RenderCommandEncoder *encoder) {
    encoder->setRenderPipelineState(PSO);
    encoder->setDepthStencilState(depthStencilState);
    encoder->setVertexBytes(&viewProjectionMatrix, sizeof(viewProjectionMatrix), NS::UInteger(viewProjectionBufferIndex));
    for (Model& model : modelList) {
        model.renderModel(encoder);
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
    PSO = device->newRenderPipelineState(descriptor, &error);
    
    vertexDescriptor->release();
    descriptor->release();
    vertexShader->release();
    fragmentShader->release();
}
