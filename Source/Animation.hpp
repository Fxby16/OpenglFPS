#pragma once

#include <Model.hpp>
#include <Bone.hpp>

#include <glm.hpp>

#include <vector>
#include <string>
#include <map>

struct AssimpNodeData{
    glm::mat4 transformation;
    std::string name;
    std::vector<AssimpNodeData> children;
};

class Animation{
public:
    Animation(const std::string& animationPath, Model& model, unsigned int animIndex, float ticksPerSecond = 0.0f);
    Animation(const aiScene* scene, Model& model, unsigned int animIndex, float ticksPerSecond = 0.0f);

    Bone* FindBone(const std::string& name);

    inline float GetDuration() const { return m_Duration; }
    inline float GetTicksPerSecond() const { return m_TicksPerSecond; }
    inline glm::mat4 GetGlobalInverseTransform() const { return m_GlobalInverseTransform; }
    inline std::vector<Bone>& GetBones() { return m_Bones; }
    inline std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
    inline AssimpNodeData& GetRootNode() { return m_RootNode; }

private:
    void ReadBones(const aiAnimation* animation, Model& model);
    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

    glm::mat4 m_GlobalInverseTransform;

    float m_Duration;
    float m_TicksPerSecond;
    AssimpNodeData m_RootNode;
    std::vector<Bone> m_Bones;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};