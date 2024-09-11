#include <Animator.hpp>
#include <ResourceManager.hpp>
#include <Log.hpp>

#include <gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Animator::Animator(const std::string& animationPath, Model& model, float ticksPerSecond)
    : m_CurrentTime(0.0f), m_DeltaTime(0.0f)
{
    m_FinalBoneMatrices.reserve(100);

    for(unsigned int i = 0; i < 100; i++){
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    AddAnimation(animationPath, model, ticksPerSecond);
}

void Animator::Update(float deltaTime)
{
    m_DeltaTime = deltaTime;
    m_CurrentTime += m_DeltaTime * m_Animations[m_CurrentAnimationIndex].GetTicksPerSecond();
    
    if(m_ShouldLoop){
        m_CurrentTime = fmod(m_CurrentTime, m_Animations[m_CurrentAnimationIndex].GetDuration()); // Loop animation
    }

    if(m_CurrentTime < m_Animations[m_CurrentAnimationIndex].GetDuration()){
        m_IsPlaying = true;
        CalculateBoneTransform(m_Animations[m_CurrentAnimationIndex].GetRootNode(), glm::mat4(1.0f));
    }else{
        m_IsPlaying = false;
    }

    // if the next frame the animation will have finished, just set it now so it doesn't break
    if(m_CurrentTime + m_DeltaTime * m_Animations[m_CurrentAnimationIndex].GetTicksPerSecond() >= m_Animations[m_CurrentAnimationIndex].GetDuration()){
        m_CurrentTime = m_Animations[m_CurrentAnimationIndex].GetDuration() + 1;
    }
}

void Animator::AddAnimation(const std::string& animationPath, Model& model, float ticksPerSecond)
{
    LogMessage("Loading animation %s", animationPath.c_str());

    m_AnimationsInfo.push_back({animationPath, ticksPerSecond});

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        LogError(importer.GetErrorString());
        return;
    }

    for(unsigned int i = 0; i < scene->mNumAnimations; i++){
        m_Animations.push_back(Animation(scene, model, i, ticksPerSecond));
    }
}

void Animator::PlayAnimation()
{
    m_CurrentTime = 0.0f;
    m_IsPlaying = true;
}

void Animator::CalculateBoneTransform(const AssimpNodeData& node, glm::mat4 parent_transform)
{
    glm::mat4 nodeTransform = node.transformation;

    Bone* bone = m_Animations[m_CurrentAnimationIndex].FindBone(node.name);

    if(bone){
        bone->Update(m_CurrentTime);
        nodeTransform = bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parent_transform * nodeTransform;

    auto& boneInfoMap = m_Animations[m_CurrentAnimationIndex].GetBoneInfoMap();
    if(boneInfoMap.find(node.name) != boneInfoMap.end()){
        int index = boneInfoMap[node.name].id;
        glm::mat4 offset = boneInfoMap[node.name].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset * m_Animations[m_CurrentAnimationIndex].GetGlobalInverseTransform();
    }

    for(auto& child : node.children){
        CalculateBoneTransform(child, globalTransformation);
    }
}

void Animator::UploadFinalBoneMatrices()
{
    Shader& shader = GetGBufferShader();
    shader.Bind();

    if(m_IsPlaying){
        for(unsigned int i = 0; i < m_FinalBoneMatrices.size(); i++){
            std::string uniformName = "finalBonesMatrices[" + std::to_string(i) + "]";
            shader.SetUniformMat4fv(uniformName, m_FinalBoneMatrices[i]);
        } 
    }

    shader.SetUniform1i("isPlaying", m_IsPlaying);
}

void Animator::UploadFinalBoneMatrices(Shader& shader)
{
    shader.Bind();

    if(m_IsPlaying){
        for(unsigned int i = 0; i < m_FinalBoneMatrices.size(); i++){
            std::string uniformName = "finalBonesMatrices[" + std::to_string(i) + "]";
            shader.SetUniformMat4fv(uniformName, m_FinalBoneMatrices[i]);
        } 
    }

    shader.SetUniform1i("isPlaying", m_IsPlaying);
}

void Animator::SetCurrentAnimation(unsigned int index)
{
    if(index < m_Animations.size()){
        m_CurrentAnimationIndex = index;
    }
}