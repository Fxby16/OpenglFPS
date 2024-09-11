#include <Animation.hpp>
#include <Log.hpp>
#include <Utils.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <queue>

Animation::Animation(const std::string& animationPath, Model& model, unsigned int animIndex, float ticksPerSecond)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        LogError(importer.GetErrorString());
        return;
    }

    m_GlobalInverseTransform = glm::inverse(AiToGlm(scene->mRootNode->mTransformation));

    aiAnimation* animation = scene->mAnimations[animIndex];

    m_Duration = animation->mDuration;
    m_TicksPerSecond = (ticksPerSecond == 0.0f) ? animation->mTicksPerSecond : ticksPerSecond;      

    ReadHierarchyData(m_RootNode, scene->mRootNode);
    ReadBones(animation, model);
}

Animation::Animation(const aiScene* scene, Model& model, unsigned int animIndex, float ticksPerSecond)
{
    m_GlobalInverseTransform = glm::inverse(AiToGlm(scene->mRootNode->mTransformation));

    aiAnimation* animation = scene->mAnimations[animIndex];

    m_Duration = animation->mDuration;
    m_TicksPerSecond = (ticksPerSecond == 0.0f) ? animation->mTicksPerSecond : ticksPerSecond;      

    ReadHierarchyData(m_RootNode, scene->mRootNode);
    ReadBones(animation, model);
}

Bone* Animation::FindBone(const std::string& name)
{
    for(Bone& bone : m_Bones){
        if(bone.GetName() == name){
            return &bone;
        }
    }

    return nullptr;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
    dest.name = src->mName.C_Str();
    dest.transformation = AiToGlm(src->mTransformation);

    for(unsigned int i = 0; i < src->mNumChildren; i++){
        AssimpNodeData newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

void Animation::ReadBones(const aiAnimation* animation, Model& model)
{
    m_BoneInfoMap = model.GetBoneInfoMap();

    for(unsigned int i = 0; i < animation->mNumChannels; i++){
        aiNodeAnim* channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.C_Str();

        m_Bones.push_back(Bone(boneName, m_BoneInfoMap[boneName].id, channel));
    }
}