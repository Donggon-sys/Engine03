//
//  extension.mm
//  Engine03
//
//  Created by Chenruyi on 2026/4/9.
//

#include "extension.h"

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <Metal/Metal.h>
#include <QuartzCore/CAMetalLayer.h>

void setLayer(GLFWwindow *window, CA::MetalLayer *layer) {
    @autoreleasepool {
        NSWindow *nswindow = glfwGetCocoaWindow(window);
        nswindow.contentView.layer = (__bridge CAMetalLayer *)layer;
        nswindow.contentView.wantsLayer = YES;
    }
}

void enterWorkSpace(GLFWwindow *window) {
    @autoreleasepool {
        NSWindow *nswindow = glfwGetCocoaWindow(window);
        [nswindow toggleFullScreen:nil];
    }
}
