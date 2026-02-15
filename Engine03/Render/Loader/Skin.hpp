//
//  Skin.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/14.
//

#pragma once
#include <vector>
#include <simd/simd.h>
#include <Metal/Metal.hpp>
//#include <tinygltf/tiny_gltf.h>

class Skin {
private:
    std::vector<unsigned int> joints;
    MTL::Buffer *pJointMatrices;
    MTL::Buffer *pInverseBindMatrices;
    bool hasInitJoinsMatrices;
    
public:
    Skin();
    ~Skin();
    Skin(const Skin&other) = delete;
    Skin &operator=(const Skin&other) = delete;
    Skin(Skin&&other);
    Skin &operator=(Skin&&other);
    void createInverseBindMatrices(MTL::Device *device, std::vector<simd::float4x4> v);
    void createJointMatrices(MTL::Device *device, std::vector<simd::float4x4> v);
};
