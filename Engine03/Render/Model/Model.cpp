//
//  Model.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#include "Model.hpp"

namespace mtlgltf {
BoundingBox::BoundingBox() {
    
}

BoundingBox::BoundingBox(simd::float3 min, simd::float3 max) : min(min), max(max) {
    
}

BoundingBox BoundingBox::getAABB(simd::float4x4 m) {
    simd::float3 min = simd::make_float3(m.columns[3]);
    simd::float3 max = min;
    simd::float3 v0, v1;
    
    simd::float3 right = simd::make_float3(m.columns[0]);
    v0 = right * this->min.x;
    v1 = right * this->max.x;
    min += simd::min(v0, v1);
    max += simd::max(v0, v1);
    
    simd::float3 up = simd::make_float3(m.columns[1]);
    v0 = up * this->min.y;
    v1 = up * this->max.y;
    min += simd::min(v0, v1);
    max += simd::max(v0, v1);
    
    simd::float3 back = simd::make_float3(m.columns[2]);
    v0 = back * this->min.z;
    v1 = back * this->max.z;
    min += simd::min(v0, v1);
    max += simd::max(v0, v1);

    return BoundingBox(min, max);
}

void Texture::updateDescriptor() {
    
}

void Texture::destroy() {
    image->release();
    sampler->release();
}

void Texture::fromgltfImage(tinygltf::Image &gltfimage, std::string path, TextureSampler textureSampler, MTL::Device *device, MTL::CommandQueue *queue) {
    unsigned char *pixelData = nullptr;
    size_t pixelDataSize = 0;
    bool needDelete = false;
    MTL::PixelFormat format = MTL::PixelFormatRGBA8Unorm;
    
    if (gltfimage.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        format = MTL::PixelFormatRGBA16Unorm;
        if (gltfimage.component == 3) {
            pixelDataSize = gltfimage.width * gltfimage.height * 8;
            pixelData = new unsigned char[pixelDataSize];
            uint16_t *rgba = (uint16_t *)pixelData;
            uint16_t *rgb = (uint16_t *)gltfimage.image.data();
            
            for (int i = 0; i < gltfimage.width * gltfimage.height; i++) {
                rgba[0] = rgb[0];
                rgba[1] = rgb[1];
                rgba[2] = rgb[2];
                rgba[3] = 65535;
                rgb = rgb + 3;
                rgba = rgba + 4;
            }
            needDelete = true;
            
        } else {
            pixelData = &gltfimage.image[0];
            pixelDataSize = gltfimage.image.size();
        }
        
    } else {
        if (gltfimage.component == 3) {
            pixelDataSize = gltfimage.width * gltfimage.height * 4;
            pixelData = new unsigned char[pixelDataSize];
            unsigned char *rgba = pixelData;
            unsigned char *rgb = gltfimage.image.data();
            
            for (int i = 0; i < gltfimage.width * gltfimage.height; i++) {
                rgba[0] = rgb[0];
                rgba[1] = rgb[1];
                rgba[2] = rgb[2];
                rgba[3] = 255;
                rgb = rgb + 3;
                rgba = rgba + 4;
            }
            needDelete = true;
            
        } else {
            pixelData = &gltfimage.image[0];
            pixelDataSize = gltfimage.image.size();
        }
    }
    
    width = gltfimage.width;
    height = gltfimage.height;
    mipLevels = static_cast<uint32_t>(floor(log2(std::max(width, height))) + 1.0);
    
    // TODO: 创建Sampler State -->sampler
    MTL::SamplerDescriptor *samplerDes = MTL::SamplerDescriptor::alloc()->init();
    samplerDes->setMagFilter(textureSampler.magFilter);
    samplerDes->setMinFilter(textureSampler.minFilter);
    samplerDes->setSAddressMode(textureSampler.addressModeU);
    samplerDes->setTAddressMode(textureSampler.addressModeV);
    samplerDes->setRAddressMode(textureSampler.addressModeW);
    sampler = device->newSamplerState(samplerDes);
    samplerDes->release();
    
    // TODO: 创建image -->texture
    MTL::TextureDescriptor *textureDes = MTL::TextureDescriptor::alloc()->init();
    textureDes->setWidth(NS::UInteger(width));
    textureDes->setHeight(NS::UInteger(height));
    textureDes->setPixelFormat(format);
    textureDes->setTextureType(MTL::TextureType2D);
    textureDes->setStorageMode(MTL::StorageModePrivate);
    textureDes->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsagePixelFormatView);
    textureDes->setMipmapLevelCount(NS::UInteger(mipLevels));
    image = device->newTexture(textureDes);
    textureDes->release();
    
    MTL::Region region(0, 0, 0, width, height, 1);
    size_t bytesPerRow = (format == MTL::PixelFormatRGBA16Unorm)? width * 8 : width * 4;
    image->replaceRegion(region, 0, 0, pixelData, NS::UInteger(bytesPerRow), 0);
    
    if (needDelete) {
        delete [] pixelData;
    }
    
    if (mipLevels > 1) {
        MTL::CommandBuffer *cmb = queue->commandBuffer();
        MTL::BlitCommandEncoder *encoder = cmb->blitCommandEncoder();
        encoder->generateMipmaps(image);
        encoder->endEncoding();
        cmb->commit();
        cmb->waitUntilCompleted();
    }
}

Primitive::Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material &material) :
    firstIndex(firstIndex), indexCount(indexCount), vertexCount(vertexCount), material(material) {
    hasIndices = indexCount > 0;
}

void Primitive::setBoundingBox(simd::float3 min, simd::float3 max) {
    bb.min = min;
    bb.max = max;
    bb.valid = true;
}

Mesh::Mesh(simd::float4x4 matrix) {
    this->matrix = matrix;
}

Mesh::~Mesh() {
    for (Primitive *p : primitives) {
        delete p;
    }
}

void Mesh::setBoundingBox(simd::float3 min, simd::float3 max) {
    bb.min = min;
    bb.max = max;
    bb.valid = true;
}

simd::float4x4 Node::localMatrix() {
    if (!useCacheMatrix) {
        simd::float4x4 t = simd::float4x4(1.0f);
        t.columns[3][0] = this->translation[0];
        t.columns[3][1] = this->translation[1];
        t.columns[3][2] = this->translation[2];
        
        simd::float4x4 r = simd::float4x4(this->rotation);
        
        simd::float4x4 s = simd::float4x4(1.0f);
        s.columns[0][0] = this->scale[0];
        s.columns[1][1] = this->scale[1];
        s.columns[2][2] = this->scale[2];
        cachedLocalMatrix = t * r * s * this->matrix;
    }
    return cachedLocalMatrix;
}

simd::float4x4 Node::getMatrix() {
    if (!useCacheMatrix) {
        simd::float4x4 m = localMatrix();
        Node *p = parent;
        while (p) {
            m = p->localMatrix() * m;
            p = p->parent;
        }
        cachedMatrix = m;
        useCacheMatrix = true;
        return m;
    } else {
        return cachedMatrix;
    }
}

void Node::update() {
    useCacheMatrix = false;
    if (mesh) {
        simd::float4x4 m = getMatrix();
        if (skin) {
            mesh->matrix = m;
            simd::float4x4 inverseTransform = simd::inverse(m);
            size_t numJoints = std::min((uint32_t)skin->joints.size(), MAX_NUM_JOINTS);
            for (size_t i = 0; i < numJoints; i++) {
                Node *jointNode = skin->joints[i];
                simd::float4x4 jointMat = jointNode->getMatrix() * skin->inverseBindMatrices[i];
                jointMat = inverseTransform * jointMat;
                mesh->jointMatrix[i] = jointMat;
            }
            mesh->jointCount = static_cast<uint32_t>(numJoints);
        } else {
            mesh->matrix = m;
        }
    }
    for (auto& child : children) {
        child->update();
    }
}

Node::~Node() {
    if (mesh) {
        delete mesh;
    }
    for (auto& child : children) {
        delete child;
    }
}

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
        case AnimationSampler::InterpolationType::LINEAR: {
            float u = std::max(0.0f, time - inputs[index]) / (inputs[index + 1] - inputs[index]);
            simd::float4 vecU{u, u, u, u};
            node->translation = simd::mix(outputsVec4[index], outputsVec4[index + 1], vecU).xyz;
            break;
        }
        case AnimationSampler::InterpolationType::STEP: {
            node->translation = outputsVec4[index].xyz;
            break;
        }
        case AnimationSampler::InterpolationType::CUBICSPLINE: {
            node->translation = cubicSplineInterpolation(index, time, 3).xyz;
            break;
        }
        default:
            break;
    }
}

void AnimationSampler::scale(size_t index, float time, Node *node) {
    switch (interpolation) {
        case AnimationSampler::InterpolationType::LINEAR: {
            float u = std::max(0.0f, time - inputs[index]) / (inputs[index + 1] - inputs[index]);
            simd::float4 vecU{u, u, u, u};
            node->scale = simd::mix(outputsVec4[index], outputsVec4[index + 1], vecU).xyz;
            break;
        }
        case AnimationSampler::InterpolationType::STEP: {
            node->scale = outputsVec4[index].xyz;
            break;
        }
        case AnimationSampler::InterpolationType::CUBICSPLINE: {
            node->scale = cubicSplineInterpolation(index, time, 3).xyz;
            break;
        }
        default:
            break;
    }
}

void AnimationSampler::rotate(size_t index, float time, Node *node) {
    switch (interpolation) {
        case AnimationSampler::InterpolationType::LINEAR: {
            float u = std::max(0.0f, time - inputs[index]) / (inputs[index + 1] - inputs[index]);
            simd::quatf q1 = simd::quatf(outputsVec4[index].x, outputsVec4[index].y, outputsVec4[index].z, outputsVec4[index].w);
            simd::quatf q2 = simd::quatf(outputsVec4[index + 1].x, outputsVec4[index + 1].y, outputsVec4[index + 1].z, outputsVec4[index + 1].w);
            node->rotation = simd::normalize(simd::slerp(q1, q2, u));
            break;
        }
        case AnimationSampler::InterpolationType::STEP: {
            node->rotation = simd::normalize(simd::quatf(outputsVec4[index].x, outputsVec4[index].y, outputsVec4[index].z, outputsVec4[index].w));
            break;
        }
        case AnimationSampler::InterpolationType::CUBICSPLINE: {
            simd::float4 rot = cubicSplineInterpolation(index, time, 4);
            node->rotation = simd::normalize(simd::quatf(rot.x, rot.y, rot.z, rot.w));
            break;
        }
        default:
            break;
    }
}

}
