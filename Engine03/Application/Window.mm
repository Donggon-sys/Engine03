//
//  Window.mm
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#include "Window.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

void Window::run() {
    int width, height;
    while (!glfwWindowShouldClose(pWindow)) {
        glfwGetWindowSize(pWindow, &width, &height);
        
        pRender->changeSize(&width, &height);
        pRender->draw(pLayer);
        glfwPollEvents();
    }
}

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pWindow = glfwCreateWindow(800, 600, "window", NULL, NULL);
    
    if (!pWindow) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    pLayer = CA::MetalLayer::layer();
    pDevice = MTL::CreateSystemDefaultDevice();
    pLayer->setDevice(pDevice);
    pLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    
    setCAMetalLayer();
    pRender = new Render(pLayer);
}

Window::~Window() {
    pDevice->release();
    pLayer->release();
    delete pRender;
    glfwTerminate();
}

void Window::setCAMetalLayer() {
    NSWindow *window = glfwGetCocoaWindow(pWindow);
    window.contentView.layer = (__bridge CAMetalLayer *)pLayer;
    window.contentView.wantsLayer = YES;
}
