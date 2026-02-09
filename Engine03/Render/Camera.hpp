//
//  Camera.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/9.
//

#pragma once
#include <simd/simd.h>

class Camera {
private:
    float fovy;
    float aspect;
    float zNear;
    float zFar;
    
    simd::float3 position;
    simd::float3 worldUP;
    simd::float3 up;
    simd::float3 center;
    simd::float3 front;
    
    simd::float4x4 lookAt(simd::float3 eye, simd::float3 center, simd::float3 up);
    simd::float4x4 projection(float fovy, float aspect, float zNear, float zFar);
    
public:
    Camera();
    ~Camera();
    void setAspect(float A);
    simd::float4x4 getViewProjectionMatrix();
    
    void goForward();
    void goBack();
    void moveLeft();
    void moveRight();
};
