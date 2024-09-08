#pragma once

#include <Animation.hpp>

class Animator{
public:
    Animator(Animation& animation);

    void Update(float deltaTime);
    void PlayAnimation();
    void CalculateBoneTransform(const AssimpNodeData& node, glm::mat4 parent_transform);

    inline std::vector<glm::mat4>& GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
    void UploadFinalBoneMatrices();

    inline void SetLooping(bool shouldLoop) { m_ShouldLoop = shouldLoop; }
    inline bool IsPlaying() { return m_CurrentTime < m_Animation.GetDuration(); }

private:
    Animation& m_Animation;
    float m_CurrentTime;
    float m_DeltaTime;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    bool m_ShouldLoop = false;
};