//
//  Scene.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#pragma once

#include <vector>
#include <simd/simd.h>
#include "Model.hpp"

class Scene {
private:
    std::vector<Model> modelList;
    simd::float4x4 viewProjectionMatrix;
    
public:
    Scene();
    ~Scene();
    
    void setViewProjectionMatrix(simd::float4x4 matrix);
    void createScene(MTL::Device *device, MTL::Library *library);
    void renderScene(MTL::RenderCommandEncoder *encoder);
};
