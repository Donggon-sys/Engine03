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
    Model m1 = Model();
    m1.build(device);
    
    modelList.push_back(std::move(m1));
}

void Scene::renderScene(MTL::RenderCommandEncoder *encoder) {
    encoder->setRenderPipelineState(PSO);
    for (Model& model : modelList) {
        model.renderModel(encoder);
    }
    encoder->setVertexBytes(&viewProjectionMatrix, sizeof(viewProjectionMatrix), NS::UInteger(1));
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
}

void Scene::setViewProjectionMatrix(simd::float4x4 matrix) {
    viewProjectionMatrix = matrix;
}

void Scene::createPipelineState(MTL::Device *device, MTL::Library *library) {
    MTL::Function *vertexShader = library->newFunction(NS::String::string("vertexShader", NS::UTF8StringEncoding));
    MTL::Function *fragmentShader = library->newFunction(NS::String::string("fragmentShader", NS::UTF8StringEncoding));
    
    MTL::RenderPipelineDescriptor *descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    descriptor->setLabel(NS::String::string("model Rendering Pipeline", NS::UTF8StringEncoding));
    descriptor->setVertexFunction(vertexShader);
    descriptor->setFragmentFunction(fragmentShader);
    descriptor->colorAttachments()->object(NS::UInteger(0))->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

    NS::Error *error;
    PSO = device->newRenderPipelineState(descriptor, &error);
    descriptor->release();
    vertexShader->release();
    fragmentShader->release();
}
