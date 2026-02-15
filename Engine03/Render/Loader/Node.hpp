//
//  Node.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#pragma once
#include <vector>
#include <simd/simd.h>

#include "Mesh.hpp"
#include "Skin.hpp"

class Node {
private:
    Node *parent;
    std::vector<Node *> children;
    uint index;
    
    simd::float3 translation;
    simd::quatf  rotation;
    simd::float3 scale;
    simd::float4x4 matrix;
    
    Mesh *mesh;
    Skin *skin;
    uint skinIndex;
    
    bool useCacheMatrix;
    simd::float4x4 cacheLocalMatrix;
    simd::float4x4 cacheMatrix;
    
public:
    Node();
    ~Node();
    Node(Node &other) = delete;
    Node& operator=(Node &oter) = delete;
    
    Node(Node &&other);
    Node& operator=(Node &&other);
    
    void addChildren(Node *child);
    simd::float4x4 getLocalMatrix();
    simd::float4x4 getWorldMatrix();
};
