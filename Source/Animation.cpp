#include <Animation.hpp>
#include <Log.hpp>
#include <Utils.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <queue>

Animation::Animation(const std::string& animationPath, Model& model, float ticksPerSecond)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        LogError(importer.GetErrorString());
        return;
    }

    m_GlobalInverseTransform = glm::inverse(AiToGlm(scene->mRootNode->mTransformation));

    m_Duration.resize(scene->mNumAnimations);
    m_TicksPerSecond.resize(scene->mNumAnimations);
    m_RootNode.resize(scene->mNumAnimations);
    m_Bones.resize(scene->mNumAnimations);
    m_BoneInfoMap.resize(scene->mNumAnimations);
    

    m_NumAnimations = scene->mNumAnimations;

    for(unsigned int i = 0; i < scene->mNumAnimations; i++){
        aiAnimation* animation = scene->mAnimations[i];

        m_Duration[i] = animation->mDuration;
        m_TicksPerSecond[i] = (ticksPerSecond == 0.0f) ? animation->mTicksPerSecond : ticksPerSecond;      

        ReadHierarchyData(m_RootNode[i], scene->mRootNode);
        ReadBones(animation, model, i);
    }
}

Bone* Animation::FindBone(const std::string& name)
{
    for(Bone& bone : m_Bones[m_CurrentAnimation]){
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

void Animation::ReadBones(const aiAnimation* animation, Model& model, unsigned int index)
{
    m_BoneInfoMap[index] = model.GetBoneInfoMap();

    for(unsigned int i = 0; i < animation->mNumChannels; i++){
        aiNodeAnim* channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.C_Str();

        m_Bones[index].push_back(Bone(boneName, m_BoneInfoMap[index][boneName].id, channel));
    }
}