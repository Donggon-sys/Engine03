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
    double x;
    double y;
};

class Window {
private:
    GLFWwindow *pWindow;
    CA::MetalLayer *pLayer;
    MTL::Device *pDevice;
    RenderAdapter *pRender;
    double detlaTime;
    
    double lastTime;
    
    // mouse
    double lastMouseX, lastMouseY;
    double currentMouseX, currentMouseY;
    
    // window
    int lastWidth, lastHeight;
    int currentWidth, currentHeight;
    bool isInit;
    bool hasFullScreen = false;
    bool isOpenItme = false;
    
    void setCAMetalLayer();
    bool shouldDraw();
    void processInput();
    void enterFullScreen();
    
//    Center getCenterPosition();
//    void getMousePos();
//    void setMousePointPosition(Center center);
    
public:
    void run();
    Window();
    ~Window();
};
