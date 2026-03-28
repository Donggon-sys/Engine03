//
//  Application.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/3/25.
//

#include "Application.hpp"

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
    
    // window
    glfwInit();
}

Application::~Application() {
    if (pDevice) {
        pDevice->release();
    }
    if (pMetalLayer) {
        pMetalLayer->release();
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
    glfwPollEvents();
    fullScreen();
    glfwPollEvents();
    setWindowLayer();
    return true;
}

void Application::setWindowLayer() {
    NSWindow *window = glfwGetCocoaWindow(pWindow);
    window.contentView.layer = (__bridge CAMetalLayer *)pMetalLayer;
    window.contentView.wantsLayer = YES;
}

void Application::runLoop() {
    if (!initWindow()) {
        return;
    }
    
    while (!glfwWindowShouldClose(pWindow)) {
        glfwPollEvents();
    }
}

MTL::Device *Application::getDevice() {
    return pDevice;
}

CA::MetalLayer *Application::getMetalLayer() {
    return pMetalLayer;
}

void Application::fullScreen() {
    NSWindow *window = glfwGetCocoaWindow(pWindow);
    [window toggleFullScreen:nil];
}

}
}
