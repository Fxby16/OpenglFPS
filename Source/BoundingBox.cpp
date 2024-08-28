#include <BoundingBox.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtc/matrix_transform.hpp>
#include <gtx/matrix_decompose.hpp>

OBB OBBFromAABB(const AABB& aabb, glm::mat4 modelMatrix)
{
    std::vector<glm::vec3> aabbVertices = {
        glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z),
        glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z),
        glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z),
        glm::vec3(aabb.max.x, aabb.max.y, aabb.max.z)
    };

    std::vector<glm::vec3> transformedVertices;
    for(const auto& vertex : aabbVertices){
        glm::vec4 transformedVertex = modelMatrix * glm::vec4(vertex, 1.0f);
        transformedVertices.push_back(glm::vec3(transformedVertex));
    }

    glm::vec3 center(0.0f);
    for(const auto& vertex : transformedVertices){
        center += vertex;
    }
    center /= static_cast<float>(transformedVertices.size());

    glm::vec3 scale;
    glm::quat rotationQuat;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(modelMatrix, scale, rotationQuat, translation, skew, perspective);

    glm::mat3 rotation = glm::mat3_cast(rotationQuat);

    glm::vec3 extents(0.0f);
    for(const auto& vertex : transformedVertices){
        glm::vec3 diff = vertex - center;
        for(int i = 0; i < 3; i++){
            float distance = glm::dot(diff, rotation[i]);
            extents[i] = std::max(extents[i], distance);
        }
    }

    return OBB(center, extents, rotation);
}

AABB AABBFromOBB(const OBB& obb)
{
    glm::vec3 min = obb.center - obb.extents;
    glm::vec3 max = obb.center + obb.extents;

    return AABB(min, max);
}