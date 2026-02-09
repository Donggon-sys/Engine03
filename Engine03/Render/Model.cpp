//
//  Model.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#include "Model.hpp"
#include <simd/simd.h>

Model::Model() {
    
}

Model::~Model() {
    pModelPSO->release();
    pModelBuffer->release();
}

Model::Model(Model&&other) {
    pModelPSO = other.pModelPSO;
    pModelBuffer = other.pModelBuffer;
    
    other.pModelBuffer = nullptr;
    other.pModelPSO = nullptr;
}

Model& Model::operator=(Model &&other) {
    if (this != &other) {
        if (pModelPSO) {
            pModelPSO->release();
        }
        if (pModelBuffer) {
            pModelBuffer->release();
        }
        
        pModelPSO = other.pModelPSO;
        pModelBuffer = other.pModelBuffer;
        
        other.pModelBuffer = nullptr;
        other.pModelPSO = nullptr;
    }
    return *this;
}

void Model::build(MTL::Device *device, MTL::Library *library) {
    createBuffer(device);
    createPipelineState(device, library);
}

void Model::renderModel(MTL::RenderCommandEncoder *encoder, simd::float4x4 matrix) {
    encoder->setRenderPipelineState(pModelPSO);
    encoder->setVertexBuffer(pModelBuffer, NS::UInteger(0), NS::UInteger(0));
    encoder->setVertexBytes(&matrix, sizeof(matrix), NS::UInteger(1));
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
}

void Model::createPipelineState(MTL::Device *device, MTL::Library *library) {
    MTL::Function *vertexShader = library->newFunction(NS::String::string("vertexShader", NS::UTF8StringEncoding));
    MTL::Function *fragmentShader = library->newFunction(NS::String::string("fragmentShader", NS::UTF8StringEncoding));
    
    MTL::RenderPipelineDescriptor *descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    descriptor->setLabel(NS::String::string("model Rendering Pipeline", NS::UTF8StringEncoding));
    descriptor->setVertexFunction(vertexShader);
    descriptor->setFragmentFunction(fragmentShader);
    descriptor->colorAttachments()->object(NS::UInteger(0))->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

    NS::Error *error;
    pModelPSO = device->newRenderPipelineState(descriptor, &error);
    descriptor->release();
    vertexShader->release();
    fragmentShader->release();
}

void Model::createBuffer(MTL::Device *device) {
    simd::float3 triangleVertices[] = {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    
    pModelBuffer = device->newBuffer(&triangleVertices, sizeof(triangleVertices), MTL::StorageModeShared);
}
