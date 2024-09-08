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
    Animation(const std::string& animationPath, Model& model, float ticksPerSecond = 0.0f);

    Bone* FindBone(const std::string& name);

    inline float GetDuration() const { return m_Duration[m_CurrentAnimation]; }
    inline float GetTicksPerSecond() const { return m_TicksPerSecond[m_CurrentAnimation]; }
    inline glm::mat4 GetGlobalInverseTransform() const { return m_GlobalInverseTransform; }
    inline std::vector<Bone>& GetBones() { return m_Bones[m_CurrentAnimation]; }
    inline std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap[m_CurrentAnimation]; }
    inline AssimpNodeData& GetRootNode() { return m_RootNode[m_CurrentAnimation]; }

    inline void SetCurrentAnimation(unsigned int index){ if(index < m_NumAnimations) m_CurrentAnimation = index; }
    inline unsigned int GetCurrentAnimation(){ return m_CurrentAnimation; }
    inline unsigned int GetNumAnimations(){ return m_NumAnimations; }

private:
    void ReadBones(const aiAnimation* animation, Model& model, unsigned int index);
    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

    glm::mat4 m_GlobalInverseTransform;
    unsigned int m_NumAnimations;
    unsigned int m_CurrentAnimation = 0;

    std::vector<float> m_Duration;
    std::vector<float> m_TicksPerSecond;
    std::vector<AssimpNodeData> m_RootNode;
    std::vector<std::vector<Bone>> m_Bones;
    std::vector<std::map<std::string, BoneInfo>> m_BoneInfoMap;
};