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
    pModelBuffer->release();
}

Model::Model(Model&&other) {
    pModelBuffer = other.pModelBuffer;
    other.pModelBuffer = nullptr;
}

Model& Model::operator=(Model &&other) {
    if (this != &other) {
        if (pModelBuffer) {
            pModelBuffer->release();
        }
        
        pModelBuffer = other.pModelBuffer;
        other.pModelBuffer = nullptr;
    }
    return *this;
}

void Model::build(MTL::Device *device) {
    createBuffer(device);
}

void Model::renderModel(MTL::RenderCommandEncoder *encoder) {
    encoder->setVertexBuffer(pModelBuffer, NS::UInteger(0), NS::UInteger(0));
    
}

void Model::createBuffer(MTL::Device *device) {
    simd::float3 triangleVertices[] = {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    
    pModelBuffer = device->newBuffer(&triangleVertices, sizeof(triangleVertices), MTL::StorageModeShared);
}
