//
//  Mesh.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/14.
//

#include "Mesh.hpp"

Mesh::Mesh() {
    pPositionBuffer   = nullptr;
    pIndexBuffer      = nullptr;
    pNormalBuffer     = nullptr;
    pTexCoord00Buffer = nullptr;
    pTexCoord01Buffer = nullptr;
    
    pWeightBuffer     = nullptr;
    pJointsBuffer     = nullptr;
}

Mesh::~Mesh() {
    pPositionBuffer->release();
    pIndexBuffer->release();
    pNormalBuffer->release();
    pTexCoord00Buffer->release();
    pTexCoord01Buffer->release();
    
    pWeightBuffer->release();
    pJointsBuffer->release();
}

Mesh::Mesh(Mesh&&other) {
    pPositionBuffer   = other.pPositionBuffer;
    pIndexBuffer      = other.pIndexBuffer;
    pNormalBuffer     = other.pNormalBuffer;
    pTexCoord00Buffer = other.pTexCoord00Buffer;
    pTexCoord01Buffer = other.pTexCoord01Buffer;
    pWeightBuffer     = other.pWeightBuffer;
    pJointsBuffer     = other.pJointsBuffer;
    
    other.pPositionBuffer   = nullptr;
    other.pIndexBuffer      = nullptr;
    other.pNormalBuffer     = nullptr;
    other.pTexCoord00Buffer = nullptr;
    other.pTexCoord01Buffer = nullptr;
    other.pWeightBuffer     = nullptr;
    other.pJointsBuffer     = nullptr;
}

Mesh &Mesh::operator=(Mesh&&other) {
    if (this != &other) {
        pPositionBuffer   = other.pPositionBuffer;
        pIndexBuffer      = other.pIndexBuffer;
        pNormalBuffer     = other.pNormalBuffer;
        pTexCoord00Buffer = other.pTexCoord00Buffer;
        pTexCoord01Buffer = other.pTexCoord01Buffer;
        pWeightBuffer     = other.pWeightBuffer;
        pJointsBuffer     = other.pJointsBuffer;
        
        other.pPositionBuffer   = nullptr;
        other.pIndexBuffer      = nullptr;
        other.pNormalBuffer     = nullptr;
        other.pTexCoord00Buffer = nullptr;
        other.pTexCoord01Buffer = nullptr;
        other.pWeightBuffer     = nullptr;
        other.pJointsBuffer     = nullptr;
    }
    return *this;
}

void Mesh::createPositionBuffer(MTL::Device *device, std::vector<simd::float3> v) {
    pPositionBuffer = device->newBuffer(v.data(), v.size() * sizeof(simd::float3), MTL::ResourceStorageModeShared);
}

void Mesh::createIndexBuffer(MTL::Device *device, std::vector<unsigned int> v) {
    pIndexBuffer = device->newBuffer(v.data(), v.size() * sizeof(unsigned int), MTL::ResourceStorageModeShared);
}

void Mesh::createNormalBuffer(MTL::Device *device, std::vector<simd::float3> v) {
    pNormalBuffer = device->newBuffer(v.data(), v.size() * sizeof(simd::float3), MTL::ResourceStorageModeShared);
}

void Mesh::createTexCoord00Buffer(MTL::Device *device, std::vector<simd::float2> v) {
    pTexCoord00Buffer = device->newBuffer(v.data(), v.size() * sizeof(simd::float2), MTL::ResourceStorageModeShared);
}

void Mesh::createTexCoord01Buffer(MTL::Device *device, std::vector<simd::float2> v) {
    pTexCoord01Buffer = device->newBuffer(v.data(), v.size() * sizeof(simd::float2), MTL::ResourceStorageModeShared);
}

void Mesh::createWeightBuffer(MTL::Device *device, std::vector<simd::float4> v) {
    pWeightBuffer = device->newBuffer(v.data(), v.size() * sizeof(simd::float4), MTL::ResourceStorageModeShared);
}

void Mesh::createJointsBuffer(MTL::Device *device, std::vector<simd::uint4> v) {
    pJointsBuffer = device->newBuffer(v.data(), v.size() * sizeof(simd::float4), MTL::ResourceStorageModeShared);
}
