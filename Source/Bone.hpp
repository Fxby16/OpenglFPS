#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <assimp/anim.h>

#include <vector>
#include <string>

struct KeyPosition{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale{
    glm::vec3 scale;
    float timeStamp;
};

class Bone{
public:
    Bone(const std::string& name, int id, const aiNodeAnim* channel);

    void Update(float animationTime);

    inline glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
    inline std::string GetName() const { return m_Name; }
    inline int GetID() const { return m_ID; }

    int GetPositionIndex(float animationTime);
    int GetRotationIndex(float animationTime);
    int GetScaleIndex(float animationTime);

private:

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime);

    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;

    glm::mat4 m_LocalTransform;
    std::string m_Name;
    int m_ID;
};