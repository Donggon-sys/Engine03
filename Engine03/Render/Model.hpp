//
//  Model.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#pragma once
#include <Metal/Metal.hpp>
#include <simd/simd.h>
#include <Foundation/Foundation.hpp>

class Model {
private:
    MTL::Buffer *pModelBuffer;

    void createBuffer(MTL::Device *device);
    
public:
    Model();
    ~Model();
    //赋值
    Model(const Model &other) = delete;
    Model& operator=(Model &other) = delete;
    //移动
    Model(Model &&other);
    Model& operator=(Model &&other);
    
    void build(MTL::Device *device);
    void renderModel(MTL::RenderCommandEncoder *encoder);
};
