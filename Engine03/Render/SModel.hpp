//
//  SModel.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/14.
//

#pragma once
#include <Metal/Metal.hpp>

class Mesh {
private:
    MTL::Buffer *pPositionBuffer;
    MTL::Buffer *pIndexBuffer;
    MTL::Buffer *pTexCoordBuffer;
    MTL::Buffer *pWeightBuffer;
    MTL::Buffer *pJointsBuffer;
    
public:
    Mesh();
    ~Mesh();
};
