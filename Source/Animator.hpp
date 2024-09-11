#pragma once

#include <Animation.hpp>

struct AnimationInfo{
    std::string path;
    float ticksPerSecond;
};

class Animator{
public:
    Animator() = default;
    Animator(const std::string& animationPath, Model& model, float ticksPerSecond = 0.0f);

    void Update(float deltaTime);
    void AddAnimation(const std::string& animationPath, Model& model, float ticksPerSecond = 0.0f);
    void SetCurrentAnimation(unsigned int index);
    void PlayAnimation();

    void CalculateBoneTransform(const AssimpNodeData& node, glm::mat4 parent_transform);

    inline std::vector<glm::mat4>& GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
    void UploadFinalBoneMatrices();
    void UploadFinalBoneMatrices(Shader& shader);

    inline void SetLooping(bool shouldLoop) { m_ShouldLoop = shouldLoop; }
    inline bool IsPlaying() { return m_IsPlaying && m_CurrentTime < m_Animations[m_CurrentAnimationIndex].GetDuration(); }
    inline std::vector<AnimationInfo>& GetAnimationsInfo() { return m_AnimationsInfo; }

private:
    std::vector<Animation> m_Animations;
    std::vector<AnimationInfo> m_AnimationsInfo;
    unsigned int m_CurrentAnimationIndex = 0;
    float m_CurrentTime;
    float m_DeltaTime;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    bool m_ShouldLoop = false;
    bool m_IsPlaying = true;
};