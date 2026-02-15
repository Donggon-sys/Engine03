//
//  Node.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#include "Node.hpp"

Node::Node():
    parent(nullptr),
    mesh(nullptr),
    skin(nullptr),
    useCacheMatrix(false),

    scale(simd::make_float3(1.0f, 1.0f, 1.0f)),
    rotation(simd::quatf(0.0f, 0.0f, 0.0f, 1.0f)),
    translation(simd::make_float3(0.0f, 0.0f, 0.0f)),
    matrix(simd::float4x4(1.0f)),

    index(0),
    skinIndex(-1) {
        
}

Node::~Node() {
    for (Node* child : children) {
        delete child;
    }
}

Node::Node(Node &&other):
    parent(other.parent),
    mesh(other.mesh),
    skin(other.skin),
    useCacheMatrix(other.useCacheMatrix),
    children(std::move(other.children)),

    scale(other.scale),
    rotation(other.rotation),
    translation(other.translation),
    matrix(other.matrix),

    index(other.index),
    skinIndex(other.skinIndex) {
        for (Node *child : children) {
            child->parent = this;
        }
        other.parent = nullptr;
        other.children.clear();
}

Node& Node::operator=(Node &&other) {
    if (this != &other) {
        // TODO: 清理自身
        for (Node *child : children) {
            delete child;
        }
        children.clear();
        
        parent = other.parent;
        mesh = other.mesh;
        skin = other.skin;
        useCacheMatrix = other.useCacheMatrix;
        children = std::move(other.children);

        scale = other.scale;
        rotation = other.rotation;
        translation = other.translation;
        matrix = other.matrix;

        index = other.index;
        skinIndex = other.skinIndex;
        
        for (Node *child : children) {
            child->parent = this;
        }
        
        other.parent = nullptr;
        other.children.clear();
    }
    
    return *this;
}

void Node::addChildren(Node *child) {
    if (child) {
        child->parent = this;
        children.push_back(child);
    }
}

simd::float4x4 Node::getLocalMatrix() {
    simd::float4x4 l = simd::float4x4(1.0f);
    return l;
}

simd::float4x4 Node::getWorldMatrix() {
    simd::float4x4 l = simd::float4x4(1.0f);
    return l;
}
