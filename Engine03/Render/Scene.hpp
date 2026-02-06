//
//  Scene.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#pragma once

#include <vector>
#include "Model.hpp"

class Scene {
private:
    std::vector<Model> modelList;
    
public:
    Scene();
    ~Scene();
    
    void createScene(MTL::Device *device, MTL::Library *library);
    void renderScene(MTL::RenderCommandEncoder *encoder);
};
