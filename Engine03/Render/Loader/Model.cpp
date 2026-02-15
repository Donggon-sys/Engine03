//
//  Model.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/14.
//

#include "Model.hpp"
#include <tinygltf/tiny_gltf.h>

Model::Model() {
    pDevice = nullptr;
}

Model::~Model() {
    
}

void Model::createModel(MTL::Device *device, std::string fileName) {
    
}

std::string Model::getFileURL(std::string fileName) {
    std::string bundlePath = NS::Bundle::mainBundle()->resourcePath()->utf8String();
    return bundlePath + "/" + fileName;
}
