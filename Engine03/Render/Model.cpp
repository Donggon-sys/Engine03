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
    pIndexBuffer->release();
    pModelBuffer->release();
    pTexCoordBuffer->release();
    pTexture->release();
}

Model::Model(Model &&other) {
    pModelBuffer = other.pModelBuffer;
    other.pModelBuffer = nullptr;
    
    pIndexBuffer = other.pIndexBuffer;
    other.pIndexBuffer = nullptr;
    
    pTexCoordBuffer = other.pTexCoordBuffer;
    other.pTexCoordBuffer = nullptr;
    
    pTexture = other.pTexture;
    other.pTexture = nullptr;
    
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
        pTexture = other.pTexture;
        pTexCoordBuffer = other.pTexCoordBuffer;
        other.pTexCoordBuffer = nullptr;
        other.pTexture = nullptr;
        indexCount = other.indexCount;
    }
    return *this;
}

void Model::renderModel(MTL::RenderCommandEncoder *encoder) {
    encoder->setVertexBuffer(pModelBuffer, NS::UInteger(0), NS::UInteger(vertexPositionBufferIndex));
    encoder->setVertexBuffer(pTexCoordBuffer, NS::UInteger(0), NS::UInteger(vertexTexCoordBufferIndex));
    encoder->setFragmentTexture(pTexture, 1);
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

void Model::createTexCoordBuffer(std::vector<simd::float2>& vertexTexCoord) {
    pTexCoordBuffer = pDevice->newBuffer(vertexTexCoord.data(), vertexTexCoord.size() * sizeof(simd::float2), MTL::StorageModeShared);
}

void Model::loadModel(std::string fileURL) {
    std::map<int, std::vector<simd::float3>> vertexData;
    std::map<int, std::vector<simd::float2>> texCoordData;
    std::map<int, std::vector<unsigned int>> indicesData;
    std::map<int, std::vector<unsigned char>> imageData;
    int imageWidth = 0, imageHeight = 0;
    
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
    
    for (tinygltf::Node &node : model.nodes) {
        if (node.mesh < 0) {
            continue;
        }
        
        int meshIndex = node.mesh;
        tinygltf::Mesh &mesh = model.meshes[meshIndex];
        // TODO: 处理SRT矩阵
        simd::float4x4 scaleMatrix = simd::float4x4(1);
        if (node.scale.size() == 3) {
            scaleMatrix.columns[0][0] = node.scale[0];
            scaleMatrix.columns[1][1] = node.scale[1];
            scaleMatrix.columns[2][2] = node.scale[2];
        }
        
        simd::float4x4 rotationMatrix = simd::float4x4(1);
        if (node.rotation.size() == 4) {
            simd::quatf quat = simd::quatf(node.rotation[0],
                                           node.rotation[1],
                                           node.rotation[2],
                                           node.rotation[3]);
            rotationMatrix = simd::float4x4(quat);
        }
        
        simd::float4x4 translationMatrix = simd::float4x4(1);
        if (node.translation.size() == 3) {
            translationMatrix.columns[3][0] = node.translation[0];
            translationMatrix.columns[3][1] = node.translation[1];
            translationMatrix.columns[3][2] = node.translation[2];
        }
        
        simd::float4x4 nodeMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        
        for (tinygltf::Primitive &primitive : mesh.primitives) {
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const auto& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
                const auto& positionBufferView = model.bufferViews[positionAccessor.bufferView];
                const auto& positionBuffer = model.buffers[positionBufferView.buffer];
                
                std::vector<simd::float3> _vertices;
                const auto* vertices = reinterpret_cast<const float *>(&positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset]);
                
                for (int i= 0; i < positionAccessor.count; i++) {
                    simd::float4 pos = simd::make_float4(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], 1.0);
                    simd::float4 worldPos = nodeMatrix * pos;
                    const simd::float3&t = simd::make_float3(worldPos.x / worldPos.w, worldPos.y / worldPos.w, worldPos.z / worldPos.w);
                    _vertices.push_back(t);
                }
                
                vertexData.insert({meshIndex, _vertices});
            }
            
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const auto& Accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const auto& BufferView = model.bufferViews[Accessor.bufferView];
                const auto& Buffer = model.buffers[BufferView.buffer];
                
                std::vector<simd::float2> _texCoords;
                const auto* texCoord = reinterpret_cast<const float *>(&Buffer.data[BufferView.byteOffset + Accessor.byteOffset]);
                
                for (int i= 0; i < Accessor.count; i++) {
                    const auto&t = simd::make_float2(texCoord[i * 2], texCoord[i * 2 + 1]);
                    _texCoords.push_back(t);
                }
                
                texCoordData.insert({meshIndex, _texCoords});
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
            
            if (model.textures.size() > 0) {
                tinygltf::Texture &Texture = model.textures[0];
                
                if (Texture.source > -1) {
                    tinygltf::Image &img = model.images[Texture.source];
                    std::vector<unsigned char> imgData = img.image;
                    
                    imageWidth = img.width;
                    imageHeight = img.height;
//                    std::cout << "pixel_type: " << img.pixel_type << std::endl;
                    imageData.insert({meshIndex, imgData});
                }
            }else {
                std::cout << "没有texture" << std::endl;
            }
        }
        
    }
    if (indicesData.empty() || vertexData.empty()) {
        std::cout << "模型数据为空" << std::endl;
        return;
    }
    
    createIndexBuffer(indicesData.at(0));
    createPositionBuffer(vertexData.at(0));
    createTexCoordBuffer(texCoordData.at(0));
    indexCount = (int)indicesData.at(0).size();
    
    MTL::TextureDescriptor *textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
    textureDescriptor->setHeight(NS::UInteger(imageHeight));
    textureDescriptor->setWidth(NS::UInteger(imageWidth));
    
    pTexture = pDevice->newTexture(textureDescriptor);
    textureDescriptor->release();
    
    MTL::Region region = MTL::Region::Make2D(0, 0, imageWidth, imageHeight);
    int bytesPerPixel = 4;
    int bytesPerRow = imageWidth * bytesPerPixel;
    
    pTexture->replaceRegion(region, 0, 0, imageData.at(0).data(), NS::UInteger(bytesPerRow), NS::UInteger(0));
}
