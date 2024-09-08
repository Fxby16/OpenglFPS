#include <Animator.hpp>
#include <ResourceManager.hpp>

#include <gtc/type_ptr.hpp>

Animator::Animator(Animation& animation)
    : m_Animation(animation), m_CurrentTime(0.0f), m_DeltaTime(0.0f)
{
    m_FinalBoneMatrices.reserve(100);

    for(unsigned int i = 0; i < 100; i++){
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::Update(float deltaTime)
{
    m_DeltaTime = deltaTime;
    m_CurrentTime += m_DeltaTime * m_Animation.GetTicksPerSecond();
    
    if(m_ShouldLoop){
        m_CurrentTime = fmod(m_CurrentTime, m_Animation.GetDuration()); // Loop animation
    }

    if(m_CurrentTime <= m_Animation.GetDuration()){
        CalculateBoneTransform(m_Animation.GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation()
{
    m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData& node, glm::mat4 parent_transform)
{
    glm::mat4 nodeTransform = node.transformation;

    Bone* bone = m_Animation.FindBone(node.name);

    if(bone){
        bone->Update(m_CurrentTime);
        nodeTransform = bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parent_transform * nodeTransform;

    auto& boneInfoMap = m_Animation.GetBoneInfoMap();
    if(boneInfoMap.find(node.name) != boneInfoMap.end()){
        int index = boneInfoMap[node.name].id;
        glm::mat4 offset = boneInfoMap[node.name].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset * m_Animation.GetGlobalInverseTransform();
    }

    for(auto& child : node.children){
        CalculateBoneTransform(child, globalTransformation);
    }
}

void Animator::UploadFinalBoneMatrices()
{
    Shader& shader = GetGBufferShader();
    shader.Bind();

    if(IsPlaying()){
        for(unsigned int i = 0; i < m_FinalBoneMatrices.size(); i++){
            std::string uniformName = "finalBonesMatrices[" + std::to_string(i) + "]";
            shader.SetUniformMat4fv(uniformName, m_FinalBoneMatrices[i]);
        } 

        shader.SetUniform1i("isPlaying", 1);
    }else{
        shader.SetUniform1i("isPlaying", 0);
    }
}