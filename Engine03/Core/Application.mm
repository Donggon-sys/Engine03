//
//  Application.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/3/25.
//

#include "Application.hpp"
#include "Time.hpp"

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

namespace BTflag {
namespace core {

Application::Application() {
    // backwark
    pDevice = MTL::CreateSystemDefaultDevice();
    pMetalLayer = CA::MetalLayer::layer();
    pMetalLayer->setDevice(pDevice);
    pMetalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    pCommandQueue = pDevice->newCommandQueue();
    
    // window
    glfwInit();
    
    // comparment
    time = std::make_shared<Time>();
    
}

Application::~Application() {
    if (pDevice) {
        pDevice->release();
    }
    if (pMetalLayer) {
        pMetalLayer->release();
    }
    if (pCommandQueue) {
        pCommandQueue->release();
    }
    glfwTerminate();
}

bool Application::initWindow() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    pWindow = glfwCreateWindow(800, 600, "window", NULL, NULL);

    
    if (!pWindow) {
        glfwTerminate();
        return false;
    }
    enterFullScreen();
    setWindowLayer();
    return true;
}

void Application::setWindowLayer() {
    @autoreleasepool {
        NSWindow *window = glfwGetCocoaWindow(pWindow);
        window.contentView.layer = (__bridge CAMetalLayer *)pMetalLayer;
        window.contentView.wantsLayer = YES;
    }
}

void Application::runLoop() {
    if (!initWindow()) {
        return;
    }

    while (!glfwWindowShouldClose(pWindow)) {
        glfwPollEvents();
        if (!hasFullScreen) {
            hasFullScreen = true;
            enterFullScreen();
        }
    }
}

MTL::Device *Application::getDevice() {
    return pDevice;
}

CA::MetalLayer *Application::getMetalLayer() {
    return pMetalLayer;
}

void Application::enterFullScreen() {
    glfwMaximizeWindow(pWindow);
}

}
}
