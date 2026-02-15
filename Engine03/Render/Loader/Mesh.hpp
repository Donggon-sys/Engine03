//
//  Mesh.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/14.
//

#pragma once
#include <vector>
#include <simd/simd.h>
#include <Metal/Metal.hpp>
//#include <tinygltf/tiny_gltf.h>

class Mesh {
private:
    MTL::Buffer *pPositionBuffer;
    MTL::Buffer *pIndexBuffer;
    MTL::Buffer *pNormalBuffer;
    MTL::Buffer *pTexCoord00Buffer;
    MTL::Buffer *pTexCoord01Buffer;
    
    MTL::Buffer *pWeightBuffer;
    MTL::Buffer *pJointsBuffer;
    
public:
    Mesh();
    ~Mesh();
    Mesh(const Mesh&other) = delete;
    Mesh &operator=(const Mesh&other) = delete;
    Mesh(Mesh&&other);
    Mesh &operator=(Mesh&&other);
    void createPositionBuffer(MTL::Device *device, std::vector<simd::float3> v);
    void createIndexBuffer(MTL::Device *device, std::vector<unsigned int> v);
    void createNormalBuffer(MTL::Device *device, std::vector<simd::float3> v);
    void createTexCoord00Buffer(MTL::Device *device, std::vector<simd::float2> v);
    void createTexCoord01Buffer(MTL::Device *device, std::vector<simd::float2> v);
    void createWeightBuffer(MTL::Device *device, std::vector<simd::float4> v);
    void createJointsBuffer(MTL::Device *device, std::vector<simd::uint4> v);
};
