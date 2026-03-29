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
#include "Render.hpp"

struct Center {
    uint x;
    uint y;
};

class Window {
private:
    GLFWwindow *pWindow;
    CA::MetalLayer *pLayer;
    MTL::Device *pDevice;
    RenderAdapter *pRender;
    double lastTime;
    double lastMouseX;
    double lastMouseY;
    bool isInit;
    bool hasFullScreen = false;
    
    void setCAMetalLayer();
    bool shouldDraw();
    void processInput();
    void enterFullScreen();
    
    Center getCenterPosition();
    
public:
    void run();
    Window();
    ~Window();
};
