//
//  Model.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/5.
//

#pragma once
#include <Metal/Metal.hpp>
#include <simd/simd.h>
#include <vector>
#include <map>
#include <string>
#include <Foundation/Foundation.hpp>

#include "Shader/ShaderType.h"

class Model {
private:
    MTL::Buffer *pModelBuffer = nullptr;
    MTL::Buffer *pIndexBuffer = nullptr;
    MTL::Buffer *pTexCoordBuffer = nullptr;
    MTL::Texture *pTexture = nullptr;
    MTL::Device *pDevice;
    int indexCount;

    void createPositionBuffer(std::vector<simd::float3>& vertexPosition);
    void createTexCoordBuffer(std::vector<simd::float2>& vertexTexCoord);
    void createIndexBuffer(std::vector<unsigned int>& vertexIndices);
    std::string getFileURL(std::string fileName);
    void loadModel(std::string fileURL);
    
public:
    Model(MTL::Device *device);
    ~Model();
    //赋值
    Model(const Model &other) = delete;
    Model& operator=(Model &other) = delete;
    //移动
    Model(Model &&other);
    Model& operator=(Model &&other);
    
    void renderModel(MTL::RenderCommandEncoder *encoder);
    void openFile(std::string fileName);
};
