//
//  Skin.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/14.
//

#include "Skin.hpp"

Skin::Skin() {
    pInverseBindMatrices = nullptr;
    pJointMatrices = nullptr;
    hasInitJoinsMatrices = false;
}

Skin::~Skin() {
    pInverseBindMatrices->release();
    pJointMatrices->release();
}

Skin::Skin(Skin&&other) {
    pInverseBindMatrices = other.pInverseBindMatrices;
    pJointMatrices = other.pJointMatrices;
    other.pInverseBindMatrices = nullptr;
    other.pJointMatrices = nullptr;
    hasInitJoinsMatrices = other.hasInitJoinsMatrices;
}

Skin &Skin::operator=(Skin&&other) {
    if (this != &other) {
        pInverseBindMatrices->release();
        pJointMatrices->release();
        pInverseBindMatrices = other.pInverseBindMatrices;
        pJointMatrices = other.pJointMatrices;
        other.pInverseBindMatrices = nullptr;
        other.pJointMatrices = nullptr;
        hasInitJoinsMatrices = other.hasInitJoinsMatrices;
    }
    return *this;
}

void Skin::createInverseBindMatrices(MTL::Device *device, std::vector<simd::float4x4> v) {
    pInverseBindMatrices = device->newBuffer(v.data(), v.size() * sizeof(simd::float4x4), MTL::ResourceStorageModeShared);
}

void Skin::createJointMatrices(MTL::Device *device, std::vector<simd::float4x4> v) {
    if (!hasInitJoinsMatrices) {
        pJointMatrices = device->newBuffer(v.data(), v.size() * sizeof(simd::float4x4), MTL::ResourceStorageModeShared);
        hasInitJoinsMatrices = true;
    } else {
        void *contents = pJointMatrices->contents();
        memcpy(contents, v.data(), v.size() * sizeof(simd::float4x4));
    }
}
