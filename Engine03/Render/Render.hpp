//
//  Render.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#pragma once
#include <QuartzCore/CAMetalLayer.hpp>

class Render {
private:
    
public:
    Render();
    ~Render();
    void draw(CA::MetalLayer *layer);
};
