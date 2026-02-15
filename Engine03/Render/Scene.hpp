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
#include "Shader/ShaderType.h"

class Scene {
private:
    std::vector<SModel> modelList;
    simd::float4x4 viewProjectionMatrix;
    MTL::RenderPipelineState *PSO;
    MTL::DepthStencilState *depthStencilState;
    
    void createDepthStencilState(MTL::Device *device);
    void createPipelineState(MTL::Device *device, MTL::Library *library);
    
public:
    Scene();
    ~Scene();
    
    void setViewProjectionMatrix(simd::float4x4 matrix);
    void createScene(MTL::Device *device, MTL::Library *library);
    void renderScene(MTL::RenderCommandEncoder *encoder);
};
