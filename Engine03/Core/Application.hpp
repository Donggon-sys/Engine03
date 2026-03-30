//
//  Application.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/3/25.
//

#pragma once
#include <memory>
struct GLFWwindow;
namespace MTL {
class Device;
class CommandQueue;
}

namespace CA {
class MetalLayer;
}

namespace BTflag {
namespace core {

class Time;

class Application {
private:
    bool hasFullScreen = false;
    GLFWwindow *pWindow = nullptr;
    MTL::Device *pDevice = nullptr;
    CA::MetalLayer *pMetalLayer = nullptr;
    MTL::CommandQueue *pCommandQueue = nullptr;
    
    std::shared_ptr<Time> time;
    
public:
    Application();
    ~Application();
    [[nodiscard]] bool initWindow();
    void initMouse();
    void setWindowLayer();
    void runLoop();
    MTL::Device *getDevice();
    CA::MetalLayer *getMetalLayer();
    void enterFullScreen();
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(const Application&&) = delete;
    Application& operator=(const Application&&) = delete;
};

}
}

