//
//  Application.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/3/25.
//

#pragma once
struct GLFWwindow;
namespace MTL {
class Device;
}

namespace CA {
class MetalLayer;
}

namespace BTflag {
namespace core {

class Application {
private:
    GLFWwindow *pWindow = nullptr;
    MTL::Device *pDevice = nullptr;
    CA::MetalLayer *pMetalLayer = nullptr;
    
public:
    Application();
    ~Application();
    bool initWindow();
    void setWindowLayer();
    void runLoop();
    MTL::Device *getDevice();
    CA::MetalLayer *getMetalLayer();
    void fullScreen();
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(const Application&&) = delete;
    Application& operator=(const Application&&) = delete;
};

}
}

