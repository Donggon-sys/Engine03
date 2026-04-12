//
//  skybox.cpp
//  Engine03
//
//  Created by Chenruyi on 2026/4/10.
//

#include "skybox.hpp"
#include <iostream>

namespace BTflag {
namespace Skybox {

void Texture::fromgltfImage(tinygltf::Image &gltfimage, std::string path, TextureSampler textureSampler, MTL::Device *device, MTL::CommandQueue *queue) {
    
    uint32_t width, height;
    uint32_t mipLevels;
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
    samplerDes->setMipFilter(MTL::SamplerMipFilterLinear);
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
//        textureDes->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsagePixelFormatView);
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

void Texture::destroy() {
    image->release();
    sampler->release();
}

Primitive::Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material &material) :
    firstIndex(firstIndex), indexCount(indexCount), vertexCount(vertexCount), material(material) {
    hasIndices = indexCount > 0;
}

Mesh::Mesh(simd::float4x4 matrix) {
    this->matrix = matrix;
}

Mesh::~Mesh() {
    for (Primitive *p : primitives) {
        delete p;
    }
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
        mesh->matrix = m;
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

Model::Model() {
    
}

Model::~Model() {
    pIndicesBuffer->release();
    pPositionBuffer->release();
    pNormalBuffer->release();
    pTexCoord0Buffer->release();
    
    for (Texture texure : textures) {
        texure.destroy();
    }
    textures.resize(0);
    textureSamplers.resize(0);
    for (Node *node : nodes) {
        delete node;
    }
    materials.resize(0);
    nodes.resize(0);
    linearNodes.resize(0);
    extensions.resize(0);
}

Model::Model(Model &&other) {
    aabb = other.aabb;
    dimensions = other.dimensions;
    nodes = std::move(other.nodes);
    linearNodes = std::move(other.linearNodes);
    textures = std::move(other.textures);
    textureSamplers = std::move(other.textureSamplers);
    materials = std::move(other.materials);
    extensions = std::move(other.extensions);
    
    pIndicesBuffer = other.pIndicesBuffer;
    pPositionBuffer = other.pPositionBuffer;
    pNormalBuffer = other.pNormalBuffer;
    pTexCoord0Buffer = other.pTexCoord0Buffer;
    pDevice = other.pDevice;
    
    other.pDevice = nullptr;
    other.pIndicesBuffer = nullptr;
    other.pPositionBuffer = nullptr;
    other.pNormalBuffer = nullptr;
    other.pTexCoord0Buffer = nullptr;
}

Model& Model::operator=(Model &&other) {
    if (this != &other) {
        aabb = other.aabb;
        dimensions = other.dimensions;
        nodes = std::move(other.nodes);
        linearNodes = std::move(other.linearNodes);
        textures = std::move(other.textures);
        textureSamplers = std::move(other.textureSamplers);
        materials = std::move(other.materials);
        extensions = std::move(other.extensions);
        
        pIndicesBuffer = other.pIndicesBuffer;
        pPositionBuffer = other.pPositionBuffer;
        pNormalBuffer = other.pNormalBuffer;
        pTexCoord0Buffer = other.pTexCoord0Buffer;
        pDevice = other.pDevice;
        
        other.pDevice = nullptr;
        other.pIndicesBuffer = nullptr;
        other.pPositionBuffer = nullptr;
        other.pNormalBuffer = nullptr;
        other.pTexCoord0Buffer = nullptr;
    }
    return *this;
}

// 下面的程序主要是进行node结构体的创建过程，node的index，node的parent，childrens这些
// 里面的hasSkin这个不是很正确，应该是hasBone才正确一点
// Skin 不是 Bone，Skin 是蒙皮配置（包含 inverseBindMatrices + joints 索引数组）-- kimi提供的消息，所以说我上面的想法是错误的

void Model::loadNode(Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, LoaderInfo &loaderInfo, float globalscale) {
    Node *newNode = new Node{ };
    newNode->index = nodeIndex;
    newNode->parent = parent;
    newNode->name = node.name;
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
        newNode->matrix = simd::float4x4(simd::make_float4((float)m[0], (float)m[1], (float)m[2], (float)m[3]),
                                         simd::make_float4((float)m[4], (float)m[5], (float)m[6], (float)m[7]),
                                         simd::make_float4((float)m[8], (float)m[9], (float)m[10], (float)m[11]),
                                         simd::make_float4((float)m[12], (float)m[13], (float)m[14], (float)m[15]));
    }
    
    // 带Children的Node
    if (node.children.size() > 0) {
        for (size_t i = 0; i < node.children.size(); i++) {
            loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, loaderInfo, globalscale);
        }
    }
    
    if (node.mesh > -1) {
        const tinygltf::Mesh mesh = model.meshes[node.mesh];
        Mesh *newMesh = new Mesh(newNode->matrix);
        for (size_t j = 0; j < mesh.primitives.size(); j++) {
            const tinygltf::Primitive &primitive = mesh.primitives[j];
            uint32_t vertexStart = static_cast<uint>(loaderInfo.vertexPos);
            uint32_t indexStart = static_cast<uint>(loaderInfo.indexPos);
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
                const float *bufferTangents = nullptr;
                const float *bufferTexCoordSet0 = nullptr;
                const float *bufferTexCoordSet1 = nullptr;
                const float *bufferColorSet0 = nullptr;
                const void *bufferJoints = nullptr;
                const float *bufferWeights = nullptr;
                
                int posBufferStride = 0;
                int normBufferStride = 0;
                int tangentBufferStride = 0;
                int uv0BufferStride = 0;
                int uv1BufferStride = 0;
                int color0BufferStride = 0;
                int jointBufferStride = 0;
                int weightBufferStride = 0;
                
                int jointComponentType = 0;
                
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
                
                // TODO: 角度
                if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
                    const tinygltf::Accessor &tangentAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView &tangentView = model.bufferViews[tangentAccessor.bufferView];
                    bufferTangents = reinterpret_cast<const float *>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
                    tangentBufferStride = tangentAccessor.ByteStride(tangentView) ? (tangentAccessor.ByteStride(tangentView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
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
                    // 确保使用正确的数据指针
                    bufferJoints = &(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
                    jointComponentType = accessor.componentType;
                    jointBufferStride = accessor.ByteStride(view) ? (accessor.ByteStride(view) / tinygltf::GetComponentSizeInBytes(jointComponentType)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
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
                    
                    loaderInfo.vertexPos++;
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
                                vertexIndices.push_back(buf[i] + vertexStart);
                                loaderInfo.indexPos++;
                            }
                            break;
                        }
                            
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                            const uint16_t *buf = static_cast<const uint16_t *>(dataPtr);
                            for (int i = 0; i < accessor.count; i++) {
                                vertexIndices.push_back(buf[i] + vertexStart);
                                loaderInfo.indexPos++;
                            }
                            break;
                        }
                            
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                            const uint8_t *buf = static_cast<const uint8_t *>(dataPtr);
                            for (int i = 0; i < accessor.count; i++) {
                                vertexIndices.push_back(buf[i] + vertexStart);
                                loaderInfo.indexPos++;
                            }
                            break;
                        }
                            
                        default:
                            break;
                    }
                }
                Primitive *newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
                newMesh->primitives.push_back(newPrimitive);
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
        }
        if (mat.values.find("baseColorFactor") != mat.values.end()) {
            auto factor = mat.values["baseColorFactor"].ColorFactor();
            material.baseColorFactor = simd::make_float4(static_cast<float>(factor[0]), static_cast<float>(factor[1]), static_cast<float>(factor[2]), static_cast<float>(factor[3]));
        }
        material.index = static_cast<uint32_t>(materials.size());
        materials.push_back(material);
    }
    materials.push_back(Material( ));
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

void Model::loadModel(MTL::Device *device, std::string fileName, MTL::CommandQueue *queue, float scale) {
    std::string bundlePath = NS::Bundle::mainBundle()->resourcePath()->utf8String();
    std::string filePath = bundlePath + "/" + fileName;
    
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error, warning;
    LoaderInfo loaderInfo{};
    pDevice = device;
    
    bool rel = loader.LoadBinaryFromFile(&model, &error, &warning, filePath);
    if (rel) {
        loadTextureSamplers(model);
        loadTexture(model, pDevice, queue);
        loadMaterials(model);
        
        const tinygltf::Scene &scene = model.scenes[model.defaultScene > 1 ? model.defaultScene : 0];

        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node &node = model.nodes[scene.nodes[i]];
            loadNode(nullptr, node, scene.nodes[i], model, loaderInfo, scale);
        }
        
        uint32_t meshindex = 0;
        for (auto node : linearNodes) {
            if (node->mesh) {
                node->mesh->index = meshindex++;
                node->update();
            }
        }
    } else {
        std::cout << "模型加载失败! " << std::endl;
        return;
    }

    pPositionBuffer = pDevice->newBuffer(position.data(), position.size() * sizeof(simd::float3), MTL::ResourceStorageModeShared);
    pNormalBuffer = pDevice->newBuffer(normal.data(), normal.size() * sizeof(simd::float3), MTL::ResourceStorageModeShared);
    pTexCoord0Buffer = pDevice->newBuffer(uv0.data(), uv0.size() * sizeof(simd::float2), MTL::ResourceStorageModeShared);
    pIndicesBuffer = pDevice->newBuffer(vertexIndices.data(), vertexIndices.size() * sizeof(unsigned int), MTL::ResourceStorageModeShared);
    
    clearup();
}

void Model::clearup() {
    position.clear();
    normal.clear();
    uv0.clear();
    vertexIndices.clear();
}

void Model::drawNode(Node *node, MTL::RenderCommandEncoder *pEncoder) {
    if (node->mesh) {
        // TODO: 动画数据进入
        simd::float4x4 transformMatrix = node->getMatrix();

        pEncoder->setVertexBytes(&transformMatrix, sizeof(simd::float4x4), NS::UInteger(10));
        for (Primitive *primitive : node->mesh->primitives) {
            if (primitive->material.baseColorTexture != NULL) {
                pEncoder->setFragmentTexture(primitive->material.baseColorTexture->image, 1);
                pEncoder->setFragmentSamplerState(primitive->material.baseColorTexture->sampler, 1);
            }
            
            pEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, primitive->indexCount, MTL::IndexType::IndexTypeUInt32, pIndicesBuffer, primitive->firstIndex * sizeof(uint32_t), 1);
        }
    }
    for (auto child : node->children) {
        drawNode(child, pEncoder);
    }
}

void Model::draw(MTL::RenderCommandEncoder *pEncoder, MTL::RenderPipelineState* pipelineState, MTL::DepthStencilState* depthStencilState) {
    pEncoder->setVertexBuffer(pPositionBuffer, NS::UInteger(0), NS::UInteger(0));
    pEncoder->setVertexBuffer(pNormalBuffer, NS::UInteger(0), NS::UInteger(1));
    pEncoder->setVertexBuffer(pTexCoord0Buffer, NS::UInteger(0), NS::UInteger(2));
    for (auto &node : nodes) {
        // TODO: 绘制每个mesh
        pEncoder->setRenderPipelineState(pipelineState);
        pEncoder->setDepthStencilState(depthStencilState);
        drawNode(node, pEncoder);
    }
}

};
}
