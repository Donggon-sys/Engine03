//
//  Model.hpp
//  Engine03
//
//  Created by Chenruyi on 2026/2/23.
//

#pragma once
#include <map>
#include <vector>
#include <string>
#include <simd/simd.h>
#include <Metal/Metal.hpp>
#include <tinygltf/tiny_gltf.h>

// 默认只是使用线性插值

namespace myModel {
constexpr uint32_t MAX_NUM_JOINTS = 128;

// TODO: 关节顺序
struct Joint {
    int jointIndex; // 指向特定的骨骼,如果jointIndex == -1，那么说明这个是根节点
    std::vector<Joint *> childJoint;
};

// TODO: 动画部分的
struct jointKeyFrame {
    simd::float3 translation;
    simd::quatf rotation;
    simd::float3 scale;
    
    int jointIndex;
};

struct AnimationKeyFrame {
    float time;
    std::vector<jointKeyFrame> joint; // 内存直接吃干净,修改成-->jointindex指向需要的骨骼
};

// TODO: 需要上传到GPU上面的jointMatrices
struct jointMatrices {
    struct {
        simd::float4x4 matrix = simd::float4x4(0.0f);
    } matrix [MAX_NUM_JOINTS];
};

class Model {
private:
    float currentTime;
    Joint rootJoint;
    std::vector<AnimationKeyFrame> keyFrame;
    
    MTL::Buffer *pInverseBindMatrices = nullptr;
    
    void loadJoint(tinygltf::Model &model);
    
public:
    Model();
    ~Model();
    void openFile(std::string fileName);
    void update(float deltaTime);
    void setStartTime(float time);
};
}
