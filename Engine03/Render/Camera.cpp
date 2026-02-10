//
//  Camera.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/9.
//

#include "Camera.hpp"

Camera::Camera() {
    fovy = 65.0f * (M_PI / 180.0f);
    aspect = 1.60037518;
    zNear = 0.1;
    zFar = 100;
    sensitivity = 0.005f;
    
    position = simd::make_float3(0.0f, 0.0f, 3.0f);
    orientation = simd::quatf(0.0f, 0.0f, 0.0f, 1.0f);
}

Camera::~Camera() {
    
}

void Camera::mouse(float deltaX, float deltaY) {
    yaw -= deltaX * sensitivity;
    pitch -= deltaY * sensitivity;
    
    // 限制俯仰角，防止万向节锁和翻转
    const float limit = M_PI_2 - 0.01f;
    pitch = (pitch > limit) ? limit : (pitch < -limit) ? -limit : pitch;
    
    float cy = cosf(yaw * 0.5f), sy = sinf(yaw * 0.5f);
    float cp = cosf(pitch * 0.5f), sp = sinf(pitch * 0.5f);
    
    orientation = simd::normalize(simd::quatf(
        sp * cy,
        cp * sy,
        -sp * sy,
        cp * cy
    ));
}

simd::float3 Camera::getFront() {
    return orientation * simd::make_float3(0.0f, 0.0f, -1.0f);
}
simd::float3 Camera::getUP() {
    return orientation * simd::make_float3(0.0f, 1.0f, 0.0f);
}

simd::float3 Camera::getRight() {
    return orientation * simd::make_float3(1.0f, 0.0f, 0.0f);
}

void Camera::moveLeft() {
    position = position - simd::normalize(simd::cross(getFront(), getUP())) * 0.06f;
}

void Camera::moveRight() {
    position = position + simd::normalize(simd::cross(getFront(), getUP())) * 0.06f;
}

void Camera::goForward() {
    position = position + 0.06f * getFront();
}

void Camera::goBack() {
    position = position - 0.06f * getFront();
}

void Camera::setAspect(float A) {
    aspect = A;
}

simd::float4x4 Camera::getViewProjectionMatrix() {
    return projection(fovy, aspect, zNear, zFar) * lookAt(position, position + getFront(), getUP());
}

simd::float4x4 Camera::lookAt(simd::float3 eye, simd::float3 center, simd::float3 up) {
    simd::float3 f = simd::normalize(center - eye);
    simd::float3 s = simd::normalize(simd::cross(up, f));
    simd::float3 u = simd::cross(f, s);
    
    simd::float4x4 mat = simd::float4x4(1);
    
    mat.columns[0][0] = s[0];
    mat.columns[1][0] = s[1];
    mat.columns[2][0] = s[2];
    
    mat.columns[0][1] = u[0];
    mat.columns[1][1] = u[1];
    mat.columns[2][1] = u[2];
    
    mat.columns[0][2] = f[0];
    mat.columns[1][2] = f[1];
    mat.columns[2][2] = f[2];
    
    mat.columns[3][0] = -simd::dot(s, eye);
    mat.columns[3][1] = -simd::dot(u, eye);
    mat.columns[3][2] = -simd::dot(f, eye);
    
    return mat;
}

simd::float4x4 Camera::projection(float fovy, float aspect, float zNear, float zFar) {
    simd::float4x4 mat = simd::float4x4(0);
    
    float tanHalfFovy = tan(fovy / 2);
    mat.columns[0][0] = 1 / (aspect * tanHalfFovy);
    mat.columns[1][1] = 1 / tanHalfFovy;
    mat.columns[2][2] = zFar / (zFar - zNear);
    mat.columns[2][3] = 1;
    mat.columns[3][2] = -(zFar * zNear) / (zFar - zNear);
    
    return mat;
}
