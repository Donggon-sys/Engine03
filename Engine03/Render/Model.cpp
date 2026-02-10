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
    pIndexBuffer->release();
}

Model::Model(Model&&other) {
    pModelBuffer = other.pModelBuffer;
    other.pModelBuffer = nullptr;
    
    pIndexBuffer = other.pIndexBuffer;
    other.pIndexBuffer = nullptr;
}

Model& Model::operator=(Model &&other) {
    if (this != &other) {
        if (pModelBuffer) {
            pModelBuffer->release();
        }
        if (pIndexBuffer) {
            pIndexBuffer->release();
        }
        
        pModelBuffer = other.pModelBuffer;
        other.pModelBuffer = nullptr;
        pIndexBuffer = other.pIndexBuffer;
        other.pIndexBuffer = nullptr;
    }
    return *this;
}

void Model::build(MTL::Device *device) {
    createBuffer(device);
}

void Model::renderModel(MTL::RenderCommandEncoder *encoder) {
    encoder->setVertexBuffer(pModelBuffer, NS::UInteger(0), NS::UInteger(0));
    
//    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
    encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(3), MTL::IndexTypeUInt32, pIndexBuffer, NS::UInteger(0), NS::UInteger(1));
}

void Model::createBuffer(MTL::Device *device) {
    simd::float3 triangleVertices[] = {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    uint index[] = {
        0, 1, 2
    };
    
    pModelBuffer = device->newBuffer(&triangleVertices, sizeof(triangleVertices), MTL::StorageModeShared);
    pIndexBuffer = device->newBuffer(&index, sizeof(index), MTL::StorageModeShared);
}
