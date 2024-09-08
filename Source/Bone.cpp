#include <Bone.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel)
    : m_LocalTransform(1.0f), m_Name(name), m_ID(id)
{
    if(channel == nullptr){
        return;
    }

    m_Positions.reserve(channel->mNumPositionKeys);
    m_Rotations.reserve(channel->mNumRotationKeys);
    m_Scales.reserve(channel->mNumScalingKeys);

    for(unsigned int i = 0; i < channel->mNumPositionKeys; i++){
        aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
        float timeStamp = channel->mPositionKeys[i].mTime;
        KeyPosition data;
        data.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
        data.timeStamp = timeStamp;
        m_Positions.push_back(data);
    }

    for(unsigned int i = 0; i < channel->mNumRotationKeys; i++){
        aiQuaternion aiOrientation = channel->mRotationKeys[i].mValue;
        float timeStamp = channel->mRotationKeys[i].mTime;
        KeyRotation data;
        data.orientation = glm::quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
        data.timeStamp = timeStamp;
        m_Rotations.push_back(data);
    }

    for(unsigned int i = 0; i < channel->mNumScalingKeys; i++){
        aiVector3D aiScale = channel->mScalingKeys[i].mValue;
        float timeStamp = channel->mScalingKeys[i].mTime;
        KeyScale data;
        data.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
        data.timeStamp = timeStamp;
        m_Scales.push_back(data);
    }
}

void Bone::Update(float animationTime){
    if(m_Positions.size() == 0 || m_Rotations.size() == 0 || m_Scales.size() == 0){
        return;
    }

    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);

    m_LocalTransform = translation * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime){
    for(unsigned int i = 0; i < m_Positions.size() - 1; i++){
        if(animationTime < m_Positions[i + 1].timeStamp){
            return i;
        }
    }

    return 0;
}

int Bone::GetRotationIndex(float animationTime){
    for(unsigned int i = 0; i < m_Rotations.size() - 1; i++){
        if(animationTime < m_Rotations[i + 1].timeStamp){
            return i;
        }
    }

    return 0;
}

int Bone::GetScaleIndex(float animationTime){
    for(unsigned int i = 0; i < m_Scales.size() - 1; i++){
        if(animationTime < m_Scales[i + 1].timeStamp){
            return i;
        }
    }

    return 0;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime){
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime){
    if(m_Positions.size() == 1){
        return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
    }

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime){
    if(m_Rotations.size() == 1){
        glm::quat rotation = glm::normalize(m_Rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);

    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime){
    if(m_Scales.size() == 1){
        return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
    }

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}