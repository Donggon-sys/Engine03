//
//  RenderAdapter.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#include "RenderAdapter.hpp"

RenderAdapter::RenderAdapter() {
    pRender = new Render();
}

RenderAdapter::~RenderAdapter() {
    delete pRender;
}

void RenderAdapter::drawInCAMetalLayer(CA::MetalLayer *layer) {
//    pRender
}
