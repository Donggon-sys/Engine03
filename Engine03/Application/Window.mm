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
#include <iostream>

const double maxFrameRate = 1.0 / 60.0;

void Window::processInput() {
    if (!isInit) {
        glfwGetWindowSize(pWindow, &lastWidth, &lastHeight);
        glfwSetCursorPos(pWindow, static_cast<double>(lastWidth) / 2.0, static_cast<double>(lastHeight) / 2.0);
        lastMouseX = static_cast<double>(lastWidth) / 2.0;
        lastMouseY = static_cast<double>(lastHeight) / 2.0;
        isInit = true;
        return;
    }
    
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
    
    
    if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE)) {
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwGetCursorPos(pWindow, &currentMouseX, &currentMouseY);
//        float deltaX = static_cast<float>( lastMouseX - currentMouseX );
//        float deltaY = static_cast<float>( currentMouseY - lastMouseY );
//        pRender->mouse(deltaX, deltaY);
        lastMouseX = currentMouseX;
        lastMouseY = currentMouseY;
    } else {
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        
        // TODO: 先判断window是否改变大小
        glfwGetWindowSize(pWindow, &currentWidth, &currentHeight);
        if (currentWidth != lastWidth or currentHeight != lastHeight) {
            lastWidth = currentWidth;
            lastHeight = currentHeight;
            lastMouseX = static_cast<double>(currentWidth) / 2.0;
            lastMouseY = static_cast<double>(currentHeight) / 2.0;
            currentMouseX = static_cast<double>(currentWidth) / 2.0;
            currentMouseY = static_cast<double>(currentHeight) / 2.0;
            glfwSetCursorPos(pWindow, static_cast<double>(currentWidth) / 2.0, static_cast<double>(currentHeight) / 2.0);
            return;
        }
        
        glfwGetCursorPos(pWindow, &currentMouseX, &currentMouseY);
        float deltaX = static_cast<float>( lastMouseX - currentMouseX );
        float deltaY = static_cast<float>( currentMouseY - lastMouseY );
        // TODO: 设置死区
        if (std::abs(deltaY) <= 0.5) {
            deltaY = 0;
        }
        pRender->mouse(deltaX, deltaY);
//        std::cout << "deltaX: " << deltaX << "," << "deltaY: " << deltaY << std::endl;
        glfwSetCursorPos(pWindow, static_cast<double>(currentWidth) / 2.0, static_cast<double>(currentHeight) / 2.0);
        lastMouseX = static_cast<double>(currentWidth) / 2.0;
        lastMouseY = static_cast<double>(currentHeight) / 2.0;
    }
    
}


void Window::run() {
//    glfwMaximizeWindow(pWindow);
    int width, height;
    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(pWindow)) {
        glfwPollEvents();
        glfwGetWindowSize(pWindow, &width, &height);
        
        if (shouldDraw()) {
            processInput();
            pRender->changeSize(&width, &height);
            pRender->update(static_cast<float>(detlaTime));
            pRender->drawInCAMetalLayer(pLayer);
        }
    }
}

bool Window::shouldDraw() {
    double currentTime = glfwGetTime();
    detlaTime = currentTime - lastTime;
    
    if (detlaTime >= maxFrameRate) {
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
    NSWindow *window = glfwGetCocoaWindow(pWindow);
    [window toggleFullScreen:nil];
}

Window::Window() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pWindow = glfwCreateWindow(960, 600, "window", NULL, NULL);
//    glfwSetWindowAspectRatio(pWindow, 16, 10);
    
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
}

Window::~Window() {
    glfwTerminate();
    pDevice->release();
    pLayer->release();
    delete pRender;
}

void Window::setCAMetalLayer() {
    NSWindow *window = glfwGetCocoaWindow(pWindow);
    window.contentView.layer = (__bridge CAMetalLayer *)pLayer;
    window.contentView.wantsLayer = YES;
}
