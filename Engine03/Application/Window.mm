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

    if (!isInit) {
        Center center = getCenterPosition();
        setMousePointPosition(center);
        isInit = true;
        return;
    }
    
    double mouseX, mouseY;
    glfwGetCursorPos(pWindow, &mouseX, &mouseY);
    
    float delatX = static_cast<float>( mouseX - lastMouseX );
    float delatY = static_cast<float>( mouseY - lastMouseY );
    pRender->mouse(-delatX, delatY);
    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

void Window::setMousePointPosition(Center center) {
    glfwSetCursorPos(pWindow, center.x, center.y);
    lastMouseX = center.x;
    lastMouseY = center.y;
}

Center Window::getCenterPosition() {
    int windowX, windowY, windowWidth, windowHeight;
    Center center;
    glfwGetWindowPos(pWindow, &windowX, &windowY);
    glfwGetWindowSize(pWindow, &windowWidth, &windowHeight);
    
    center.x = windowX + windowWidth / 2;
    center.y = windowY + windowHeight / 2;
    return center;
}

void Window::windowReSize(GLFWwindow* window, int xpos, int ypos) {
    Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    Center center = self->getCenterPosition();
    self->setMousePointPosition(center);
}

void Window::run() {
    glfwSetWindowUserPointer(pWindow, this);
    glfwMaximizeWindow(pWindow);
    glfwSetWindowPosCallback(pWindow, windowReSize);
    int width, height;
    lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(pWindow)) {
        glfwPollEvents();
        glfwGetWindowSize(pWindow, &width, &height);
        
        if (shouldDraw()) {
            processInput();
            pRender->changeSize(&width, &height);
            pRender->update(static_cast<float>(1 / 60.0f));
            pRender->drawInCAMetalLayer(pLayer);
        }
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
    NSWindow *window = glfwGetCocoaWindow(pWindow);
    [window toggleFullScreen:nil];
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
    pRender = new RenderAdapter(pLayer);
    
    isInit = false;
//    glfwMaximizeWindow(pWindow);
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
