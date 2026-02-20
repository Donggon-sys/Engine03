//
//  Scene.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#pragma once

#include <vector>
#include <simd/simd.h>
#include "Model/Model.hpp"
#include "Model.hpp"
#include "Shader/ShaderType.h"
#include "Camera.hpp"

class Scene {
private:
    Camera *pCamera;
    enum MaterialType {DEFAULT};
    std::vector<SModel> smodelList;
    std::vector<mtlgltf::Model> modelList;
    simd::float4x4 viewProjectionMatrix;
    MTL::DepthStencilState *depthStencilState;
    std::map<MaterialType, MTL::RenderPipelineState *> PSOList;
    void createDepthStencilState(MTL::Device *device);
    void createPipelineState(MTL::Device *device, MTL::Library *library);
    
public:
    Scene();
    ~Scene();
    
    void setViewPortSize(simd::uint2 v);
    void goForward();
    void goBack();
    void moveLeft();
    void moveRight();
    void setMouse(float deltaX, float deltaY);
    void setViewProjectionMatrix(simd::float4x4 matrix);
    void createScene(MTL::Device *device, MTL::Library *library);
    void renderScene(MTL::RenderCommandEncoder *encoder);
};
