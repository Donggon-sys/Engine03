//
//  Model.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/23.
//

#include "Model.hpp"
#include <iostream>

namespace myModel {
Model::Model() {
    
}

Model::~Model() {
    pInverseBindMatrices->release();
}

void Model::openFile(std::string fileName) {
    std::string bundlePath = NS::Bundle::mainBundle()->resourcePath()->utf8String();
    std::string filePath = bundlePath + "/" + fileName;
    
    std::string warning, err;
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    
    if (loader.LoadBinaryFromFile(&model, &err, &warning, filePath)) {
        // TODO: 目前只加载关键帧骨骼动画数据
        // 第一步先获取根joint
        loadJoint(model);
        
    } else {
        std::cout << "打开文件失败！ " << std::endl;
        return;
    }
}

void Model::loadJoint(tinygltf::Model &model) {
    Joint *joint = new Joint();
    
//    for (<#range declaration#> : <#range expression#>) {
//        <#statements#>
//    }
}

void Model::setStartTime(float time) {
    this->currentTime = time;
}

void Model::update(float deltaTime) {
    this->currentTime += deltaTime;
}

}
