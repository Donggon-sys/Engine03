//
//  main.mm
//  Engine03
//
//  Created by Chenruyi on 2026/2/2.
//

#include "Window.hpp"
#include "../Core/Application.hpp"

int main(int argc, const char * argv[]) {
//    Window window;
//    window.run();
    BTflag::core::Application base;
    base.runLoop();
    return 0;
}
