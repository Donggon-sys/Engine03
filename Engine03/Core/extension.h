//
//  extension.h
//  Engine03
//
//  Created by Chenruyi on 2026/4/9.
//

#ifndef extension_h
#define extension_h

namespace CA {
class MetalLayer;
}
struct GLFWwindow;

void setLayer(GLFWwindow *window, CA::MetalLayer *layer);
void enterWorkSpace(GLFWwindow *window);

#endif /* extension_h */

