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

void Model::build(MTL::Device *device, MTL::RenderPipelineState *PSO) {
    createBuffer(device);
    pModelPSO = PSO;
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
