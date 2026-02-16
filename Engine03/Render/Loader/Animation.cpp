//
//  Animation.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#include "Animation.hpp"
#include <simd/simd.h>

simd::float4 AnimationSampler::cubicSplineInterpolation(size_t index, float time, uint32_t stride) {
    float delta = inputs[index + 1] - inputs[index];
    float t = (time - inputs[index]) / delta;
    const size_t current = index * stride * 3;
    const size_t next = (index + 1) * stride * 3;
    const size_t A = 0;
    const size_t V = stride * 1;
    const size_t B = stride * 2;
    
    float t2 = powf(t, 2.0f);
    float t3 = powf(t, 3.0f);
    simd::float4 pt = simd::make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (uint32_t i = 0; i < stride; i++) {
        float p0 = outputs[current + i + V];
        float m0 = delta * outputs[current + i + A];
        float p1 = outputs[next + i + V];
        float m1 = delta * outputs[next + i + B];
        pt[i] = ((2.0f * t3 - 3.0f * t2 + 1.0f) * p0) + ((t3 - 2.0f * t2 + t) * m0) + ((-2.0f * t3 + 3.0f * t2) * p1) + ((t3 - t2) * m1);
    }
    return pt;
}

void AnimationSampler::translate(size_t index, float time, Node *node) {
    switch (interpolation) {
        case AnimationSampler::interpolationType::LINEAR: {
            float u = std::max(0.0f, time - inputs[index]) / (inputs[index + 1] - inputs[index]);
            simd::float4 vecU{u, u, u, u};
            node->setTranslation(simd::mix(outputsFloat4[index], outputsFloat4[index + 1], vecU));
            break;
        }
        case AnimationSampler::interpolationType::STEP: {
            node->setTranslation(outputsFloat4[index]);
            break;
        }
        case AnimationSampler::interpolationType::CUBICSPLINE: {
            node->setTranslation(cubicSplineInterpolation(index, time, 3));
            break;
        }
        default:
            break;
    }
}

void AnimationSampler::scale(size_t index, float time, Node *node) {
    switch (interpolation) {
        case AnimationSampler::interpolationType::LINEAR: {
            float u = std::max(0.0f, time - inputs[index]) / (inputs[index + 1] - inputs[index]);
            simd::float4 vecU{u, u, u, u};
            node->setScale(simd::mix(outputsFloat4[index], outputsFloat4[index + 1], vecU));
            break;
        }
        case AnimationSampler::interpolationType::STEP: {
            node->setScale(outputsFloat4[index]);
            break;
        }
        case AnimationSampler::interpolationType::CUBICSPLINE: {
            node->setScale(cubicSplineInterpolation(index, time, 3));
            break;
        }
        default:
            break;
    }
}

void AnimationSampler::rotate(size_t index, float time, Node *node) {
    switch (interpolation) {
        case AnimationSampler::interpolationType::LINEAR: {
            float u = std::max(0.0f, time - inputs[index]) / (inputs[index + 1] - inputs[index]);
            simd::quatf q1 = simd::quatf(outputsFloat4[index].x, outputsFloat4[index].y, outputsFloat4[index].z, outputsFloat4[index].w);
            simd::quatf q2 = simd::quatf(outputsFloat4[index + 1].x, outputsFloat4[index + 1].y, outputsFloat4[index + 1].z, outputsFloat4[index + 1].w);
            node->setRotation(simd::slerp(q1, q2, u));
            break;
        }
        case AnimationSampler::interpolationType::STEP: {
            node->setRotation(simd::quatf(outputsFloat4[index].x, outputsFloat4[index].y, outputsFloat4[index].z, outputsFloat4[index].w));
            break;
        }
        case AnimationSampler::interpolationType::CUBICSPLINE: {
            simd::float4 rot = cubicSplineInterpolation(index, time, 4);
            node->setRotation(simd::quatf(rot.x, rot.y, rot.z, rot.w));
            break;
        }
        default:
            break;
    }
}
