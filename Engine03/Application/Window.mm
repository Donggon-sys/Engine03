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

const double maxFrameRate = 1.0 / 60.0;

void Window::processInput() {
    if (glfwGetKey(pWindow, GLFW_KEY_W)) {
        pRender->goForward();
    }
    if (glfwGetKey(pWindow, GLFW_KEY_S)) {
        pRender->goBack();
    }
    if (glfwGetKey(pWindow, GLFW_KEY_A)) {
        pRender->moveLeft();
    }
    if (glfwGetKey(pWindow, GLFW_KEY_D)) {
        pRender->moveRight();
    }
    
    double mouseX, mouseY;
    glfwGetCursorPos(pWindow, &mouseX, &mouseY);
    
    if (!isInit) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        isInit = true;
        return;
    }
    
    float delatX = (float)mouseX - lastMouseX;
    float delatY = (float)mouseY - lastMouseY;
    
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    
    pRender->mouse(-delatX, delatY);
}

void Window::run() {
    int width, height;
    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(pWindow)) {
        glfwGetWindowSize(pWindow, &width, &height);
        
        if (shouldDraw()) {
            processInput();
            pRender->changeSize(&width, &height);
            double currentTime = glfwGetTime();
            pRender->update(static_cast<float>(currentTime - lastTime));
            pRender->drawInCAMetalLayer(pLayer);
        }
        glfwPollEvents();
    }
}

bool Window::shouldDraw() {
    double currentTime = glfwGetTime();
    double delta = currentTime - lastTime;
    
    if (delta >= maxFrameRate) {
        //TODO: 更新lastTime
        lastTime = lastTime + maxFrameRate;
        
        if (currentTime - lastTime > maxFrameRate) {
            lastTime = currentTime;
        }
        return true;
    }
    return false;
}

void Window::enterFullScreen() {
//    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
//    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
//    
//    glfwSetWindowMonitor(pWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    
    NSWindow *window = glfwGetCocoaWindow(pWindow);
    [window toggleFullScreen:nil];
}

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pWindow = glfwCreateWindow(640, 400, "window", NULL, NULL);
    
    if (!pWindow) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    pLayer = CA::MetalLayer::layer();
    pDevice = MTL::CreateSystemDefaultDevice();
    pLayer->setDevice(pDevice);
    pLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    
    setCAMetalLayer();
    pRender = new RenderAdapter(pLayer);
    
    isInit = false;
//    glfwMaximizeWindow(pWindow);
    enterFullScreen();
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
