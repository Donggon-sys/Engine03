//
//  Animation.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#pragma once

class AnimationChannel {
private:
    
public:
    AnimationChannel() { };
    ~AnimationChannel() { };
};

class AnimationSampler {
private:
    
public:
    AnimationSampler() { };
    ~AnimationSampler() { };
};

class Animation {
private:
    AnimationChannel chanle;
    AnimationSampler sampler;
    float duration;
    
public:
    Animation() { };
    ~Animation() { };
};
