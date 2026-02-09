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
    float sensitivity;
    
    float yaw = 0.0f;
    float pitch = 0.0f;
    
    simd::float3 position;
    simd::quatf orientation;
    
    simd::float4x4 lookAt(simd::float3 eye, simd::float3 center, simd::float3 up);
    simd::float4x4 projection(float fovy, float aspect, float zNear, float zFar);
    simd::float3 getFront();
    simd::float3 getUP();
    simd::float3 getRight();
    
public:
    Camera();
    ~Camera();
    void setAspect(float A);
    simd::float4x4 getViewProjectionMatrix();
    
    void goForward();
    void goBack();
    void moveLeft();
    void moveRight();
    void mouse(float deltaX, float deltaY);
};
