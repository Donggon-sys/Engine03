//
//  Window.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#pragma once
#include <GLFW/glfw3.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "RenderAdapter.hpp"

class Window {
private:
    GLFWwindow *pWindow;
    CA::MetalLayer *pLayer;
    MTL::Device *pDevice;
    RenderAdapter *pRender;
    
    void setCAMetalLayer();
    
public:
    void run();
    Window();
    ~Window();
};
