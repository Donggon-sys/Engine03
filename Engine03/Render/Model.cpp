//
//  Model.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#include "Model.hpp"
#include <iostream>
#include <simd/simd.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>

Model::Model(MTL::Device *device) {
    pDevice = device;
}

Model::~Model() {
    if (pIndexBuffer != nullptr) {
        pIndexBuffer->release();
    }
    if (pModelBuffer != nullptr) {
        pModelBuffer->release();
    }
}

Model::Model(Model &&other) {
    pModelBuffer = other.pModelBuffer;
    other.pModelBuffer = nullptr;
    
    pIndexBuffer = other.pIndexBuffer;
    other.pIndexBuffer = nullptr;
    indexCount = other.indexCount;
}

Model& Model::operator=(Model &&other) {
    if (this != &other) {
        if (pModelBuffer) {
            pModelBuffer->release();
        }
        if (pIndexBuffer) {
            pIndexBuffer->release();
        }
        
        pModelBuffer = other.pModelBuffer;
        other.pModelBuffer = nullptr;
        pIndexBuffer = other.pIndexBuffer;
        other.pIndexBuffer = nullptr;
        indexCount = other.indexCount;
    }
    return *this;
}

void Model::renderModel(MTL::RenderCommandEncoder *encoder) {
    encoder->setVertexBuffer(pModelBuffer, NS::UInteger(0), NS::UInteger(vertexPositionBuffer));
    encoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(indexCount), MTL::IndexTypeUInt32, pIndexBuffer, NS::UInteger(0), NS::UInteger(1));
}

void Model::openFile(std::string fileName) {
    loadModel(getFileURL(fileName));
}

std::string Model::getFileURL(std::string fileName) {
    std::string bundlePath = NS::Bundle::mainBundle()->resourcePath()->utf8String();
    return bundlePath + "/" + fileName;
}

void Model::createPositionBuffer(std::vector<simd::float3>& vertexPosition) {
    pModelBuffer = pDevice->newBuffer(vertexPosition.data(), vertexPosition.size() * sizeof(simd::float3),  MTL::StorageModeShared);
}

void Model::createIndexBuffer(std::vector<unsigned int>& vertexIndices) {
    pIndexBuffer = pDevice->newBuffer(vertexIndices.data(), vertexIndices.size() * sizeof(unsigned int), MTL::StorageModeShared);
}

void Model::loadModel(std::string fileURL) {
    std::map<int, std::vector<simd::float3>> vertexData;
    std::map<int, std::vector<unsigned int>> indicesData;
    
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error, warning;
    bool rel = loader.LoadBinaryFromFile(&model, &error, &warning, fileURL);
    
    if (!error.empty()) {
        std::cout << error << std::endl;
    }
    if (!warning.empty()) {
        std::cout << warning << std::endl;
    }
    if (!rel) {
        std::cout << "加载失败！" << std::endl;
        return;
    }else {
        std::cout << "加载成功！" << std::endl;
    }
    
    int nodeIndex = -1;
    for (tinygltf::Node &node : model.nodes) {
        ++nodeIndex;
        for (tinygltf::Mesh &mesh : model.meshes) {
            int meshIndex = nodeIndex;
            for (tinygltf::Primitive &primitive : mesh.primitives) {
                if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                    const auto& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
                    const auto& positionBufferView = model.bufferViews[positionAccessor.bufferView];
                    const auto& positionBuffer = model.buffers[positionBufferView.buffer];
                    
                    std::vector<simd::float3> _vertices;
                    const auto* vertices = reinterpret_cast<const float *>(&positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset]);
                    
                    for (int i= 0; i < positionAccessor.count; i++) {
                        const auto&t = simd::make_float3(vertices[i * 3], vertices[i * 3 + 1],vertices[i * 3 + 2]);
                        _vertices.push_back(t);
                    }
                    
                    vertexData.insert({meshIndex, _vertices});
                }
                
                if (primitive.indices >= 0) {
                    const auto& indexAccessor = model.accessors[primitive.indices];
                    const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                    const auto& indexBuffer = model.buffers[indexBufferView.buffer];
                    
                    std::vector<unsigned int> _indices;
                    if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        const unsigned int *indices = reinterpret_cast<const unsigned int *>(&indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]);
                        for (int i = 0; i < indexAccessor.count; i++) {
                            _indices.push_back(indices[i]);
                        }
                    }
                    if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        const unsigned short *indices = reinterpret_cast<const unsigned short *>(&indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]);
                        for (int i = 0; i < indexAccessor.count; i++) {
                            _indices.push_back(indices[i]);
                        }
                    }
                    
                    indicesData.insert({meshIndex, _indices});
                }
            }
        }
        
    }
    if (indicesData.empty() || vertexData.empty()) {
        std::cout << "模型数据为空" << std::endl;
        return;
    }
    
    createIndexBuffer(indicesData.at(0));
    createPositionBuffer(vertexData.at(0));
    indexCount = (int)indicesData.at(0).size();
}
