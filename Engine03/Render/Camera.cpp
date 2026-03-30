//
//  Camera.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/9.
//

#include "Camera.hpp"

Camera::Camera() {
    fovy = 45.0f * (M_PI / 180.0f);
    aspect = 1.60037518;
    zNear = 0.1;
    zFar = 100;
    sensitivity = 0.005f;
    halfYaw = 0.0f;
    halfPicth = 0.0f;
    
    position = simd::make_float3(0.0f, 0.0f, 3.0f);
    orientation = simd::quatf(0.0f, 0.0f, 0.0f, 1.0f);
}

Camera::~Camera() {
    
}

void Camera::mouse(float deltaX, float deltaY) {
    halfYaw   = -deltaX * sensitivity * 0.5;
    halfPicth = -deltaY * sensitivity * 0.5;
    simd::quatf q_yaw = simd::quatf(0.0f, std::sin(halfYaw), 0.0f, std::cos(halfYaw));
    simd::quatf q_pitch = simd::quatf(std::sin(halfPicth), 0.0f, 0.0f, std::cos(halfPicth));
    
    orientation = simd::normalize(q_yaw * orientation * q_pitch);
//    orientation = simd::normalize(q_pitch * q_yaw * orientation);
}

simd::float3 Camera::getFront() {
    return orientation(simd::float3{0.0f, 0.0f, -1.0f});
}
simd::float3 Camera::getUP() {
    return orientation(simd::float3{0.0f, 1.0f, 0.0f});
}

simd::float3 Camera::getRight() {
    return orientation(simd::float3{1.0f, 0.0f, 0.0f});
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
