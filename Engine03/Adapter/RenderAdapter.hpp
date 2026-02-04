//
//  RenderAdapter.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "Render.hpp"

class RenderAdapter {
private:
    Render *pRender;
    
public:
    RenderAdapter(CA::MetalLayer *layer);
    ~RenderAdapter();
    void drawInCAMetalLayer(CA::MetalLayer *layer);
};
