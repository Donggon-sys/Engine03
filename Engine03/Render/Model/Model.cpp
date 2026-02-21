//
//  Model.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/16.
//

#include "Model.hpp"
#include <simd/simd.h>
#include <iostream>

namespace mtlgltf {
    BoundingBox::BoundingBox() {
        
    }

    BoundingBox::BoundingBox(simd::float3 min, simd::float3 max) : min(min), max(max) {
        
    }

    BoundingBox BoundingBox::getAABB(simd::float4x4 m) {
        simd::float3 min = simd::make_float3(m.columns[3][0], m.columns[3][1], m.columns[3][2]);
        simd::float3 max = min;
        simd::float3 v0, v1;
        
        simd::float3 right = simd::make_float3(m.columns[0][0], m.columns[0][1], m.columns[0][2]);
        v0 = right * this->min.x;
        v1 = right * this->max.x;
        min += simd::min(v0, v1);
        max += simd::max(v0, v1);
        
        simd::float3 up = simd::make_float3(m.columns[1][0], m.columns[1][1], m.columns[1][2]);
        v0 = up * this->min.y;
        v1 = up * this->max.y;
        min += simd::min(v0, v1);
        max += simd::max(v0, v1);
        
        simd::float3 back = simd::make_float3(m.columns[2][0], m.columns[2][1], m.columns[2][2]);
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
        textureDes->setStorageMode(MTL::StorageModeShared);
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

    Model::Model() {
        
    }

    Model::~Model() {
        pIndicesBuffer->release();
        pPositionBuffer->release();
        pNormalBuffer->release();
        pTexCoord0Buffer->release();
        pTexCoord1Buffer->release();
        pColorBuffer->release();
        pJointsBuffer->release();
        pWeightsBuffer->release();
        
        for (Texture texure : textures) {
            texure.destroy();
        }
        textures.resize(0);
        textureSamplers.resize(0);
        for (Node *node : nodes) {
            delete node;
        }
        materials.resize(0);
        animations.resize(0);
        nodes.resize(0);
        linearNodes.resize(0);
        extensions.resize(0);
        for (Skin *skin : skins) {
            delete skin;
        }
        skins.resize(0);
    }

    Model::Model(Model &&other) {
        aabb = other.aabb;
        dimensions = other.dimensions;
        nodes = std::move(other.nodes);
        linearNodes = std::move(other.linearNodes);
        skins = std::move(other.skins);
        textures = std::move(other.textures);
        textureSamplers = std::move(other.textureSamplers);
        materials = std::move(other.materials);
        animations = std::move(other.animations);
        extensions = std::move(other.extensions);
        
        pIndicesBuffer = other.pIndicesBuffer;
        pPositionBuffer = other.pPositionBuffer;
        pNormalBuffer = other.pNormalBuffer;
        pTexCoord0Buffer = other.pTexCoord0Buffer;
        pTexCoord1Buffer = other.pTexCoord1Buffer;
        pColorBuffer = other.pColorBuffer;
        pJointsBuffer = other.pJointsBuffer;
        pWeightsBuffer = other.pWeightsBuffer;
        
        other.pIndicesBuffer = nullptr;
        other.pPositionBuffer = nullptr;
        other.pNormalBuffer = nullptr;
        other.pTexCoord0Buffer = nullptr;
        other.pTexCoord1Buffer = nullptr;
        other.pColorBuffer = nullptr;
        other.pJointsBuffer = nullptr;
        other.pWeightsBuffer = nullptr;
    }

    Model& Model::operator=(Model &&other) {
        if (this != &other) {
            aabb = other.aabb;
            dimensions = other.dimensions;
            nodes = std::move(other.nodes);
            linearNodes = std::move(other.linearNodes);
            skins = std::move(other.skins);
            textures = std::move(other.textures);
            textureSamplers = std::move(other.textureSamplers);
            materials = std::move(other.materials);
            animations = std::move(other.animations);
            extensions = std::move(other.extensions);
            
            pIndicesBuffer = other.pIndicesBuffer;
            pPositionBuffer = other.pPositionBuffer;
            pNormalBuffer = other.pNormalBuffer;
            pTexCoord0Buffer = other.pTexCoord0Buffer;
            pTexCoord1Buffer = other.pTexCoord1Buffer;
            pColorBuffer = other.pColorBuffer;
            pJointsBuffer = other.pJointsBuffer;
            pWeightsBuffer = other.pWeightsBuffer;
            
            other.pIndicesBuffer = nullptr;
            other.pPositionBuffer = nullptr;
            other.pNormalBuffer = nullptr;
            other.pTexCoord0Buffer = nullptr;
            other.pTexCoord1Buffer = nullptr;
            other.pColorBuffer = nullptr;
            other.pJointsBuffer = nullptr;
            other.pWeightsBuffer = nullptr;
        }
        return *this;
    }

    void Model::loadNode(Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, float globalscale) {
        Node *newNode = new Node{ };
        newNode->index = nodeIndex;
        newNode->parent = parent;
        newNode->name = node.name;
        newNode->skinIndex = node.skin;
        newNode->matrix = simd::float4x4(1.0f);
        
        if (node.translation.size() == 3) {
            const double *v = node.translation.data();
            newNode->translation = simd::make_float3((float)v[0], (float)v[1], (float)v[2]);
        }
        if (node.rotation.size() == 4) {
            const double *v = node.rotation.data();
            newNode->rotation = simd::quatf((float)v[0], (float)v[1], (float)v[2], (float)v[3]);
        }
        if (node.scale.size() == 3) {
            const double *v = node.scale.data();
            newNode->scale = simd::make_float3((float)v[0], (float)v[1], (float)v[2]);
        }
        if (node.matrix.size() == 16) {
            const double *m = node.matrix.data();
            newNode->matrix = simd::float4x4(simd::make_float4((float)m[0], (float)m[4], (float)m[8], (float)m[12]),
                                             simd::make_float4((float)m[1], (float)m[5], (float)m[9], (float)m[13]),
                                             simd::make_float4((float)m[2], (float)m[6], (float)m[10], (float)m[14]),
                                             simd::make_float4((float)m[3], (float)m[7], (float)m[11], (float)m[15]));
        }
        
        if (node.children.size() > 0) {
            for (size_t i = 0; i < node.children.size(); i++) {
                loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, globalscale);
            }
        }
        
        if (node.mesh > -1) {
            const tinygltf::Mesh mesh = model.meshes[node.mesh];
            Mesh *newMesh = new Mesh(newNode->matrix);
            for (size_t j = 0; j < mesh.primitives.size(); j++) {
                const tinygltf::Primitive &primitive = mesh.primitives[j];
                uint32_t vertexStart = 0;
                uint32_t indexStart = 0;
                uint32_t indexCount = 0;
                uint32_t vertexCount = 0;
                simd::float3 posMin {0.0f, 0.0f, 0.0f};
                simd::float3 posMax {0.0f, 0.0f, 0.0f};
                bool hasSkin = false;
                bool hasIndices = primitive.indices > -1;
                
                // TODO: 处理顶点
                {
                    const float *bufferPos = nullptr;
                    const float *bufferNormals = nullptr;
                    const float *bufferTexCoordSet0 = nullptr;
                    const float *bufferTexCoordSet1 = nullptr;
                    const float *bufferColorSet0 = nullptr;
                    const void *bufferJoints = nullptr;
                    const float *bufferWeights = nullptr;
                    
                    int posBufferStride = 0;
                    int normBufferStride = 0;
                    int uv0BufferStride = 0;
                    int uv1BufferStride = 0;
                    int color0BufferStride = 0;
                    int jointBufferStride = 0;
                    int weightBufferStride = 0;
                    
                    int jointComponentTpye = 0;
                    
                    assert(primitive.attributes.find("POSITION") != primitive.attributes.end());
                    
                    const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
                    bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
                    posMax = simd::make_float3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
                    posMin = simd::make_float3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
                    vertexCount = static_cast<uint32_t>(posAccessor.count);
                    posBufferStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
                    
                    // TODO: 法线
                    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                        const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                        const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
                        bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                        normBufferStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
                    }
                    
                    // TODO: uv0，uv1
                    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                        const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                        const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                        bufferTexCoordSet0 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        uv0BufferStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
                    }
                    if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
                        const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
                        const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                        bufferTexCoordSet1 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        uv1BufferStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
                    }
                    
                    // TODO: 顶点色彩
                    if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                        const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
                        const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
                        bufferColorSet0 = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        color0BufferStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
                    }
                    
                    // TODO: 蒙皮；骨骼
                    if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
                        const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
                        const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
                        bufferJoints = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        jointBufferStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
                        jointComponentTpye = accessor.componentType;
                    }
                    if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
                        const tinygltf::Accessor &accessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
                        const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
                        bufferWeights = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        weightBufferStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
                    }
                    hasSkin = (bufferJoints && bufferWeights);
                    
                    // TODO: 把获取的数据丢到结构体Vertex里
                    for (size_t v = 0; v < posAccessor.count; v++) {
                        {
                            const float *pos = &bufferPos[v * posBufferStride];
                            this->position.push_back(simd::make_float3(pos[0], pos[1], pos[2]));
                        }
                        
                        if (bufferNormals) {
                            const float *pNormal = &bufferNormals[v * normBufferStride];
                            this->normal.push_back(simd::make_float3(pNormal[0], pNormal[1], pNormal[2]));
                        } else {
                            this->normal.push_back(simd::make_float3(0.0f, 0.0f, 0.0f));
                        }
                        
                        if (bufferTexCoordSet0) {
                            const float *uv0 = &bufferTexCoordSet0[v * uv0BufferStride];
                            this->uv0.push_back(simd::make_float2(uv0[0], uv0[1]));
                        } else {
                            this->uv0.push_back(simd::make_float2(0.0f, 0.0f));
                        }
                        
                        if (bufferTexCoordSet1) {
                            const float *uv1 = &bufferTexCoordSet1[v * uv1BufferStride];
                            this->uv1.push_back(simd::make_float2(uv1[0], uv1[1]));
                        } else {
                            this->uv1.push_back(simd::make_float2(0.0f, 0.0f));
                        }
                        
                        if (bufferColorSet0) {
                            const float *col = &bufferTexCoordSet1[v * uv1BufferStride];
                            this->color.push_back(simd::make_float4(col[0], col[1], col[2], col[3]));
                        } else {
                            this->color.push_back(simd::make_float4(0.0f, 0.0f, 1.0f, 1.0f));
                        }
                        
                        if (hasSkin) {
                            switch (jointComponentTpye) {
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                                    const uint16_t *buf = static_cast<const uint16_t *>(bufferJoints);
                                    {
                                        this->joint0.push_back(simd::make_uint4(buf[0], buf[1], buf[2], buf[3]));
                                    }
                                    break;
                                }
                                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                                    const uint8_t *buf = static_cast<const uint8_t *>(bufferJoints);
                                    {
                                        this->joint0.push_back(simd::make_uint4(buf[0], buf[1], buf[2], buf[3]));
                                    }
                                    break;
                                }
                                default:
                                    break;
                            }
                        } else {
                            this->joint0.push_back(simd::make_uint4(0.0f, 0.0f, 0.0f, 0.0f));
                        }
                        
                        if (hasSkin) {
                            const float *weight = &bufferWeights[v * weightBufferStride];
                            this->weight0.push_back(simd::make_float4(weight[0], weight[1], weight[2], weight[3]));
                        } else {
                            this->weight0.push_back(simd::make_float4(0.0f, 0.0f, 0.0f, 0.0f));
                        }
                        
                        if (simd::length(this->weight0.back()) == 0.0f) {
                            this->weight0.push_back(simd::make_float4(1.0f, 0.0f, 0.0f, 0.0f));
                        }
                    }
                    
                    // TODO: 处理index
                    if (hasIndices) {
                        const tinygltf::Accessor &accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
                        const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
                        const tinygltf::Buffer &buffer = model.buffers[view.buffer];
                        
                        indexCount = static_cast<uint32_t>(accessor.count);
                        const void *dataPtr = &(buffer.data[accessor.byteOffset + view.byteOffset]);
                        
                        switch (accessor.componentType) {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                                const uint32_t *buf = static_cast<const uint32_t *>(dataPtr);
                                for (int i = 0; i < accessor.count; i++) {
                                    vertexIndices.push_back(buf[i]);
                                }
                                break;
                            }
                                
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                                const uint16_t *buf = static_cast<const uint16_t *>(dataPtr);
                                for (int i = 0; i < accessor.count; i++) {
                                    vertexIndices.push_back(buf[i]);
                                }
                                break;
                            }
                                
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                                const uint8_t *buf = static_cast<const uint8_t *>(dataPtr);
                                for (int i = 0; i < accessor.count; i++) {
                                    vertexIndices.push_back(buf[i]);
                                }
                                break;
                            }
                                
                            default:
                                break;
                        }
                    }
                    Primitive *newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
                    newPrimitive->setBoundingBox(posMin, posMax);
                    newMesh->primitives.push_back(newPrimitive);
                }
                
                for (Primitive *p : newMesh->primitives) {
                    if (p->bb.valid && !newMesh->bb.valid) {
                        newMesh->bb = p->bb;
                        newMesh->bb.valid = true;
                    }
                    newMesh->bb.min = simd::min(newMesh->bb.min, p->bb.min);
                    newMesh->bb.max = simd::min(newMesh->bb.max, p->bb.max);
                }
                newNode->mesh = newMesh;
            }
        }
        if (parent) {
            parent->children.push_back(newNode);
        } else {
            nodes.push_back(newNode);
        }
        linearNodes.push_back(newNode);
        
    }

    void Model::getNodeProps(const tinygltf::Node &node, const tinygltf::Model &model, size_t &vertexCount, size_t &indexCount) {
        if (node.children.size() > 0) {
            for (size_t i = 0; i < node.children.size(); i++) {
                getNodeProps(model.nodes[node.children[i]], model, vertexCount, indexCount);
            }
        }
        if (node.mesh > -1) {
            const tinygltf::Mesh mesh = model.meshes[node.mesh];
            for (size_t i = 0; i < mesh.primitives.size(); i++) {
                const tinygltf::Primitive &primitive = mesh.primitives[i];
                vertexCount += model.accessors[primitive.attributes.find("POSITION")->second].count;
                if (primitive.indices > -1) {
                    indexCount += model.accessors[primitive.indices].count;
                }
            }
        }
    }

    void Model::loadSkin(tinygltf::Model &model) {
        for (tinygltf::Skin &source : model.skins) {
            Skin *newSkin = new Skin{ };
            newSkin->name = source.name;
            
            if (source.skeleton > -1) {
                newSkin->skeletonRoot = nodeFromIndex(source.skeleton);
            }
            
            for (int jointIndex : source.joints) {
                Node *node = nodeFromIndex(jointIndex);
                if (node) {
                    newSkin->joints.push_back(nodeFromIndex(jointIndex));
                }
            }
            
            if (source.inverseBindMatrices > -1) {
                const tinygltf::Accessor &accessor = model.accessors[source.inverseBindMatrices];
                const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = model.buffers[view.buffer];
                const float *m = reinterpret_cast<const float *>(&buffer.data[accessor.byteOffset + view.byteOffset]);
                
                for (size_t i = 0; i < accessor.count; i++) {
                    newSkin->inverseBindMatrices.push_back(simd::float4x4(simd::make_float4(m[0], m[1], m[2], m[3]),
                                                                          simd::make_float4(m[4], m[5], m[6], m[7]),
                                                                          simd::make_float4(m[8], m[9], m[10], m[11]),
                                                                          simd::make_float4(m[12], m[13], m[14], m[15])));
                    m = m + 16;
                }
            }
            
            if (newSkin->joints.size() > MAX_NUM_JOINTS) {
                std::cout << "错误!" << std::endl;
            }
            
            skins.push_back(newSkin);
        }
    }

    void Model::loadTexture(tinygltf::Model &model, MTL::Device *device, MTL::CommandQueue *queue) {
        for (tinygltf::Texture &tex : model.textures) {
            int source = tex.source;
            
            tinygltf::Image image = model.images[source];
            TextureSampler textureSampler;
            if (tex.sampler == -1) {
                textureSampler.magFilter = MTL::SamplerMinMagFilterLinear;
                textureSampler.minFilter = MTL::SamplerMinMagFilterLinear;
                textureSampler.addressModeU = MTL::SamplerAddressModeRepeat;
                textureSampler.addressModeV = MTL::SamplerAddressModeRepeat;
                textureSampler.addressModeW = MTL::SamplerAddressModeRepeat;
            } else {
                textureSampler = textureSamplers[tex.sampler];
            }
            Texture texture;
            texture.fromgltfImage(image, "", textureSampler, device, queue);
            textures.push_back(texture);
        }
    }

    MTL::SamplerMinMagFilter Model::getFilterMode(int32_t filterMode) {
        switch (filterMode) {
            case -1:
            case 9728: return MTL::SamplerMinMagFilterNearest;
            case 9729: return MTL::SamplerMinMagFilterLinear;
            case 9984: return MTL::SamplerMinMagFilterNearest;
            case 9985: return MTL::SamplerMinMagFilterNearest;
            case 9986: return MTL::SamplerMinMagFilterLinear;
            case 9987: return MTL::SamplerMinMagFilterLinear;
        }
        return MTL::SamplerMinMagFilterNearest;
    }

    MTL::SamplerAddressMode Model::getWarpMode(int32_t warpMode) {
        switch (warpMode) {
            case -1:
            case 10497: return MTL::SamplerAddressModeRepeat;
            case 33071: return MTL::SamplerAddressModeClampToEdge;
            case 33648: return MTL::SamplerAddressModeMirrorRepeat;
        }
        return MTL::SamplerAddressModeRepeat;
    }

    void Model::loadTextureSamplers(tinygltf::Model &model) {
        for (tinygltf::Sampler &smpl : model.samplers) {
            TextureSampler sampler{ };
            sampler.magFilter = getFilterMode(smpl.magFilter);
            sampler.minFilter = getFilterMode(smpl.minFilter);
            sampler.addressModeU = getWarpMode(smpl.wrapS);
            sampler.addressModeV = getWarpMode(smpl.wrapT);
            sampler.addressModeW = sampler.addressModeV;
            textureSamplers.push_back(sampler);
        }
    }

    void Model::loadMaterials(tinygltf::Model &model) {
        for (tinygltf::Material &mat : model.materials) {
            Material material{ };
            material.doubleSided = mat.doubleSided;
            if (mat.values.find("baseColorTexture") != mat.values.end()) {
                material.baseColorTexture = &textures[mat.values["baseColorTexture"].TextureIndex()];
                material.texCoordSets.baseColor = mat.values["baseColorTexture"].TextureTexCoord();
            }
            if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
                material.metallicRoughnessTexture = &textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
                material.texCoordSets.metallicRoughness = mat.values["metallicRoughnessTexture"].TextureTexCoord();
            }
            if (mat.values.find("roughnessFactor") != mat.values.end()) {
                material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
            }
            if (mat.values.find("metallicFactor") != mat.values.end()) {
                material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
            }
            if (mat.values.find("baseColorFactor") != mat.values.end()) {
                auto factor = mat.values["baseColorFactor"].ColorFactor();
                material.baseColorFactor = simd::make_float4(static_cast<float>(factor[0]), static_cast<float>(factor[1]), static_cast<float>(factor[2]), static_cast<float>(factor[3]));
            }
            if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
                material.normalTexture = &textures[mat.additionalValues["normalTexture"].TextureIndex()];
                material.texCoordSets.normal = mat.additionalValues["normalTexture"].TextureTexCoord();
            }
            if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
                material.emissiveTexture = &textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
                material.texCoordSets.emissive = mat.additionalValues["emissiveTexture"].TextureTexCoord();
            }
            if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
                material.occlusionTexture = &textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
                material.texCoordSets.occlusion = mat.additionalValues["occlusionTexture"].TextureTexCoord();
            }
            if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
                tinygltf::Parameter param = mat.additionalValues["alphaMode"];
                if (param.string_value == "BLEND") {
                    material.alphaMode = Material::ALPHAMODE_BLEND;
                }
                if (param.string_value == "MASK") {
                    material.alphaCutoff = 0.5f;
                    material.alphaMode = Material::ALPHAMODE_MASK;
                }
            }
            if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
                material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
            }
            if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
                auto factor = mat.additionalValues["emissiveFactor"].ColorFactor();
                material.emissiveFactor = simd::make_float4(static_cast<float>(factor[0]), static_cast<float>(factor[1]), static_cast<float>(factor[2]), 1.0f);
            }
            
            material.index = static_cast<uint32_t>(materials.size());
            materials.push_back(material);
        }
        materials.push_back(Material( ));
    }

    void Model::loadAnimation(tinygltf::Model &model) {
        for (tinygltf::Animation &anim : model.animations) {
            Animation animation{ };
            animation.name = anim.name;
            if (anim.name.empty()) {
                animation.name = std::to_string(animations.size());
            }
            
            for (tinygltf::AnimationSampler &samp : anim.samplers) {
                AnimationSampler sampler{ };
                
                if (samp.interpolation == "LINEAR") {
                    sampler.interpolation = AnimationSampler::InterpolationType::LINEAR;
                }
                if (samp.interpolation == "STEP") {
                    sampler.interpolation = AnimationSampler::InterpolationType::STEP;
                }
                if (samp.interpolation == "CUBICSPLINE") {
                    sampler.interpolation = AnimationSampler::InterpolationType::CUBICSPLINE;
                }
                
                // TODO: 读取采样的时间
                {
                    const tinygltf::Accessor &accessor = model.accessors[samp.input];
                    const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer &buffer = model.buffers[view.buffer];
                    
                    assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                    
                    const void *dataPtr = &buffer.data[view.byteOffset + accessor.byteOffset];
                    const float *buf = static_cast<const float *>(dataPtr);
                    for (size_t index = 0; index < accessor.count; index++) {
                        sampler.inputs.push_back(buf[index]);
                    }
                    
                    for (auto input : sampler.inputs) {
                        if (input < animation.start) {
                            animation.start = input;
                        }
                        if (input > animation.end) {
                            animation.end = input;
                        }
                    }
                }
                
                // TODO: 读取采样的SRT矩阵
                {
                    const tinygltf::Accessor &accessor = model.accessors[samp.output];
                    const tinygltf::BufferView &view = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer &buffer = model.buffers[view.buffer];
                    
                    assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                    
                    const void *dataPtr = &buffer.data[accessor.byteOffset + view.byteOffset];
                    switch (accessor.type) {
                        case TINYGLTF_TYPE_VEC3: {
                            const simd::float3 *buf = static_cast<const simd::float3 *>(dataPtr);
                            for (size_t index = 0; index < accessor.count; index++) {
                                sampler.outputsVec4.push_back(simd::make_float4(buf[index], 1.0f));
                                sampler.outputs.push_back(buf[index][0]);
                                sampler.outputs.push_back(buf[index][1]);
                                sampler.outputs.push_back(buf[index][2]);
                            }
                            break;
                        }
                        case TINYGLTF_TYPE_VEC4: {
                            const simd::float4 *buf = static_cast<const simd::float4 *>(dataPtr);
                            for (size_t index = 0; index < accessor.count; index++) {
                                sampler.outputsVec4.push_back(buf[index]);
                                sampler.outputs.push_back(buf[index][0]);
                                sampler.outputs.push_back(buf[index][1]);
                                sampler.outputs.push_back(buf[index][2]);
                                sampler.outputs.push_back(buf[index][3]);
                            }
                            break;
                        }
                        default: {
                            std::cout << "类型错误" << std::endl;
                            break;
                        }
                    }
                    animation.samplers.push_back(sampler);
                }
                
                // TODO: 动画通道
                for (tinygltf::AnimationChannel &source : anim.channels) {
                    AnimationChannel channel{ };
                    
                    if (source.target_path == "rotation") {
                        channel.path = AnimationChannel::PathType::ROTATION;
                    }
                    if (source.target_path == "translation") {
                        channel.path = AnimationChannel::PathType::TRANSLATION;
                    }
                    if (source.target_path == "scale") {
                        channel.path = AnimationChannel::PathType::SCALE;
                    }
                    if (source.target_path == "weights") {
                        std::cout << "不支持权重" << std::endl;
                    }
                    channel.samplerIndex = source.sampler;
                    channel.node = nodeFromIndex(source.target_node);
                    if (!channel.node) {
                        continue;
                    }
                    
                    animation.channels.push_back(channel);
                }
            }
            animations.push_back(animation);
        }
    }

    void Model::calculateBoundBox(Node *node, Node *parent) {
        BoundingBox pareentBVH = parent ? parent->bvh : BoundingBox(dimensions.min, dimensions.max);
        
        if (node->mesh) {
            if (node->mesh->bb.valid) {
                node->aabb = node->mesh->bb.getAABB(node->getMatrix());
                if (node->children.size() == 0) {
                    node->bvh.min = node->aabb.min;
                    node->bvh.max = node->aabb.max;
                    node->bvh.valid = true;
                }
            }
        }
        
        pareentBVH.min = simd::min(pareentBVH.min, node->bvh.min);
        pareentBVH.max = simd::min(pareentBVH.max, node->bvh.max);
        
        for (auto &child : node->children) {
            calculateBoundBox(child, node);
        }
    }

    void Model::getSceneDimensions() {
        for (Node *node : linearNodes) {
            calculateBoundBox(node, nullptr);
        }
        
        dimensions.min = simd::make_float3(FLT_MAX, FLT_MAX, FLT_MAX);
        dimensions.max = simd::make_float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        
        for (Node *node : linearNodes) {
            if (node->bvh.valid) {
                dimensions.min = simd::min(dimensions.min, node->bvh.min);
                dimensions.max = simd::min(dimensions.max, node->bvh.max);
            }
        }
        
        simd::float3 extent = dimensions.max - dimensions.min;
        aabb = simd::float4x4(simd::make_float4(extent.x, 0.0f, 0.0f, 0.0f),
                              simd::make_float4(0.0f, extent.y, 0.0f, 0.0f),
                              simd::make_float4(0.0f, 0.0f, extent.z, 0.0f),
                              simd::make_float4(dimensions.min.x, dimensions.min.y, dimensions.min.z, 1.0f));
    }

    Node* Model::fineNode(Node *parent, uint32_t index) {
        Node *nodeFound = nullptr;
        if (parent->index == index) {
            return parent;
        }
        for (auto &child : parent->children) {
            nodeFound = fineNode(child, index);
            if (nodeFound) {
                break;
            }
        }
        return nodeFound;
    }

    Node* Model::nodeFromIndex(uint32_t index) {
        Node *nodeFound = nullptr;
        for (auto &node : nodes) {
            nodeFound = fineNode(node, index);
            if (nodeFound) {
                break;
            }
        }
        return nodeFound;
    }

    void Model::updateAnimation(uint32_t index, float time) {
        if (animations.empty()) {
            std::cout << "没有动画! " << std::endl;
            return;
        }
        if (index > static_cast<uint32_t>(animations.size()) - 1) {
            std::cout << "没有带索引的动画! " << std::endl;
            return;
        }
        Animation &animation = animations[index];
        
        bool update = false;
        for (auto &channel : animation.channels) {
            AnimationSampler &sampler = animation.samplers[channel.samplerIndex];
            if (sampler.inputs.size() > sampler.outputsVec4.size()) {
                continue;
            }
            
            for (size_t i = 0; i < sampler.inputs.size() - 1; i++) {
                if ((time >= sampler.inputs[i]) && (time <= sampler.inputs[i + 1])) {
                    float u = std::max(0.0f, time - sampler.inputs[i]) / (sampler.inputs[i + 1] - sampler.inputs[i]);
                    if (u <= 1.0f) {
                        switch (channel.path) {
                            case AnimationChannel::PathType::TRANSLATION: {
                                sampler.translate(i, time, channel.node);
                                break;
                            }
                            case AnimationChannel::PathType::SCALE: {
                                sampler.scale(i, time, channel.node);
                                break;
                            }
                            case AnimationChannel::PathType::ROTATION: {
                                sampler.rotate(i, time, channel.node);
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                        update = true;
                    }
                }
            }
        }
        if (update) {
            for (auto &node : nodes) {
                node->update();
            }
        }
    }

    void Model::loadModel(MTL::Device *device, std::string fileName, MTL::CommandQueue *queue, float scale) {
        std::string bundlePath = NS::Bundle::mainBundle()->resourcePath()->utf8String();
        std::string filePath = bundlePath + "/" + fileName;
        
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string error, warning;
        size_t vertexCount = 0;
        size_t indexCount = 0;
        pDevice = device;
        
        bool rel = loader.LoadBinaryFromFile(&model, &error, &warning, filePath);
        if (rel) {
            loadTextureSamplers(model);
            loadTexture(model, pDevice, queue);
            loadMaterials(model);
            
            const tinygltf::Scene &scene = model.scenes[model.defaultScene > 1 ? model.defaultScene : 0];
            
            for (size_t index = 0; index < scene.nodes.size(); index++) {
                getNodeProps(model.nodes[scene.nodes[index]], model, vertexCount, indexCount);
            }
            
            for (size_t i = 0; i < scene.nodes.size(); i++) {
                const tinygltf::Node &node = model.nodes[scene.nodes[i]];
                loadNode(nullptr, node, scene.nodes[i], model, scale);
            }
            if (model.animations.size() > 0) {
                loadAnimation(model);
            }
            loadSkin(model);
            
            uint32_t meshindex = 0;
            for (auto node : linearNodes) {
                if (node->skinIndex > -1) {
                    node->skin = skins[node->skinIndex];
                }
                if (node->mesh) {
                    node->mesh->index = meshindex++;
                    node->update();
                }
            }
        } else {
            std::cout << "模型加载失败! " << std::endl;
            return;
        }
        
        size_t vertexBufferSize = vertexCount * sizeof(Vertex);
        
        assert(vertexBufferSize > 0);

        pPositionBuffer = pDevice->newBuffer(position.data(), position.size() * sizeof(simd::float3), MTL::ResourceStorageModeShared);
        pNormalBuffer = pDevice->newBuffer(normal.data(), normal.size() * sizeof(simd::float3), MTL::ResourceStorageModeShared);
        pTexCoord0Buffer = pDevice->newBuffer(uv0.data(), uv0.size() * sizeof(simd::float2), MTL::ResourceStorageModeShared);
        pTexCoord1Buffer = pDevice->newBuffer(uv1.data(), uv1.size() * sizeof(simd::float2), MTL::ResourceStorageModeShared);
        pJointsBuffer = pDevice->newBuffer(joint0.data(), joint0.size() * sizeof(simd::uint4), MTL::ResourceStorageModeShared);
        pWeightsBuffer = pDevice->newBuffer(weight0.data(), weight0.size() * sizeof(simd::float4), MTL::ResourceStorageModeShared);
        pColorBuffer = pDevice->newBuffer(color.data(), color.size() * sizeof(simd::float4), MTL::ResourceStorageModeShared);
        pIndicesBuffer = pDevice->newBuffer(vertexIndices.data(), vertexIndices.size() * sizeof(unsigned int), MTL::ResourceStorageModeShared);
    }

    void Model::drawNode(Node *node, MTL::RenderCommandEncoder *pEncoder) {
        if (node->mesh) {
            for (Primitive *primitive : node->mesh->primitives) {
                pEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, primitive->indexCount, MTL::IndexType::IndexTypeUInt32, pIndicesBuffer, primitive->firstIndex * sizeof(uint32_t), 1);
            }
        }
        for (auto child : node->children) {
            drawNode(child, pEncoder);
        }
    }

    void Model::draw(MTL::RenderCommandEncoder *pEncoder) {
        pEncoder->setVertexBuffer(pPositionBuffer, NS::UInteger(0), NS::UInteger(0));
        pEncoder->setVertexBuffer(pNormalBuffer, NS::UInteger(0), NS::UInteger(1));
        pEncoder->setVertexBuffer(pTexCoord0Buffer, NS::UInteger(0), NS::UInteger(2));
        pEncoder->setVertexBuffer(pTexCoord1Buffer, NS::UInteger(0), NS::UInteger(3));
        pEncoder->setVertexBuffer(pJointsBuffer, NS::UInteger(0), NS::UInteger(4));
        pEncoder->setVertexBuffer(pWeightsBuffer, NS::UInteger(0), NS::UInteger(5));
        pEncoder->setVertexBuffer(pColorBuffer, NS::UInteger(0), NS::UInteger(6));
        for (auto &node : nodes) {
            drawNode(node, pEncoder);
        }
    }

}
