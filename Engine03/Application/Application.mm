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

namespace BTflag {
namespace core {

Application::Application() = default;

Application::~Application() {
    
}

void Application::runLoop() {
    
}

MTL::Device *Application::getDevice() {
    return pDevice;
}

CA::MetalLayer *Application::getMetalLayer() {
    return pMetalLayer;
}

}
}
