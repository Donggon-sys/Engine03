//
//  Animation.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#pragma once
#include <string>
#include <vector>
#include <simd/simd.h>

#include "Node.hpp"

class AnimationChannel {
private:
    enum pathType {TRANSLATION, ROTATION, SCALE};
    pathType path;
    Node *node;
    uint32_t samplerIndex;
    
public:
    AnimationChannel() { };
    ~AnimationChannel() { };
};

class AnimationSampler {
private:
    enum interpolationType {LINEAR, STEP, CUBICSPLINE};
    interpolationType interpolation;
    std::vector<float> inputs;
    std::vector<simd::float4> outputsFloat4;
    std::vector<float> outputs;
    
public:
    AnimationSampler();
    ~AnimationSampler() {
        
    };
    simd::float4 cubicSplineInterpolation(size_t index, float time, uint32_t stride);
    void translate(size_t index, float time, Node *node);
    void scale(size_t index, float time, Node *node);
    void rotate(size_t index, float time, Node *node);
};

class Animation {
private:
    std::string name;
    std::vector<AnimationChannel> channel;
    std::vector<AnimationSampler> sampler;
    float duration;
    
public:
    Animation() { };
    ~Animation() { };
};
