//
//  Scene.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#include "Scene.hpp"

Scene::Scene() {
    
}

Scene::~Scene() {
    modelList.clear();
}

void Scene::createScene(MTL::Device *device, MTL::Library *library) {
    Model m1 = Model();
    m1.build(device, library);
    
    modelList.push_back(std::move(m1));
}

void Scene::renderScene(MTL::RenderCommandEncoder *encoder, simd::float4x4 matrix) {
    for (Model& model : modelList) {
        model.renderModel(encoder, matrix);
    }
}
