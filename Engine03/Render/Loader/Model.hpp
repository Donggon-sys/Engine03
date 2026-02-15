//
//  Model.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/14.
//

#pragma once

#include "Mesh.hpp"
#include "Skin.hpp"
#include "Animation.hpp"
#include <string>


class Model {
private:
    std::vector<Mesh> meshes;
    std::vector<Skin> skins;
    std::vector<Animation> animations;
    
    MTL::Device *pDevice;
    
    std::string getFileURL(std::string fileName);
    
public:
    Model();
    ~Model();
    void createModel(MTL::Device *device, std::string fileName);
};
