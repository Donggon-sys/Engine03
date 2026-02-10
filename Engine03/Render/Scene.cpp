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

void Scene::renderScene(MTL::RenderCommandEncoder *encoder) {
    for (Model& model : modelList) {
        model.renderModel(encoder);
    }
    encoder->setVertexBytes(&viewProjectionMatrix, sizeof(viewProjectionMatrix), NS::UInteger(1));
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
}

void Scene::setViewProjectionMatrix(simd::float4x4 matrix) {
    viewProjectionMatrix = matrix;
}
