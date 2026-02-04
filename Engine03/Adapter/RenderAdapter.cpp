//
//  RenderAdapter.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#include "RenderAdapter.hpp"

RenderAdapter::RenderAdapter(CA::MetalLayer *layer) {
    pRender = new Render(layer);
}

RenderAdapter::~RenderAdapter() {
    delete pRender;
}

void RenderAdapter::drawInCAMetalLayer(CA::MetalLayer *layer) {
//    pRender
    pRender->draw(layer);
}

void RenderAdapter::changeSize(int *width, int *height) {
    pRender->changeSize(width, height);
}
