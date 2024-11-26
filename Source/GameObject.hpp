#pragma once

#include <Shader.hpp>
#include <Globals.hpp>
#include <Physics.hpp>  

#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/matrix_decompose.hpp>
#include <Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#include <cstdint>
#include <vector>
#include <limits>

struct GameObjectTransform{
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scale;
    glm::mat4 mat;

    GameObjectTransform() : pos(0.0f), rot(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f), mat(1.0f){}
    
    GameObjectTransform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) : pos(position), rot(rotation), scale(scale){
        mat = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) * glm::scale(glm::mat4(1.0f), scale);
    }

    GameObjectTransform(glm::mat4 matrix){
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(matrix, scale, rot, pos, skew, perspective);
    }

    inline void SetPosition(glm::vec3 position){
        pos = position;
        mat = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) * glm::scale(glm::mat4(1.0f), scale);
    }

    inline void SetRotation(glm::vec3 rotation){
        rot = glm::quat(rotation);
        mat = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) * glm::scale(glm::mat4(1.0f), scale);
    }

    inline void SetScale(glm::vec3 s){
        scale = s;
        mat = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) * glm::scale(glm::mat4(1.0f), scale);
    }

    inline glm::mat4 GetMatrix(){
        return glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) * glm::scale(glm::mat4(1.0f), scale);
    }

    static inline GameObjectTransform Interpolate(GameObjectTransform& a, GameObjectTransform& b, float alpha){
        GameObjectTransform result;
        result.pos = glm::mix(a.pos, b.pos, alpha);
        result.rot = glm::slerp(a.rot, b.rot, alpha);
        result.scale = glm::mix(a.scale, b.scale, alpha);
        result.mat = glm::translate(glm::mat4(1.0f), result.pos) * glm::toMat4(result.rot) * glm::scale(glm::mat4(1.0f), result.scale);
        return result;
    }
};

class GameObject{
public:
    GameObject() = default;
    GameObject(uint32_t model_id, bool animated);

    void AddInstance(GameObjectTransform transform, JPH::BodyID body_id);
    void RemoveInstance(int index);

    void Update();
    glm::mat4 InterpolateTransforms(float alpha, int index);

    void Draw(Shader& shader);
    void DrawShadow(Shader& shader, glm::mat4 light_space_matrix);

    inline uint32_t GetModelID() const { return model_id; }
    inline bool IsAnimated() const { return animated; }
    inline std::vector<std::pair<GameObjectTransform, GameObjectTransform>>& GetTransforms() { return transforms; }
    inline glm::mat4 GetTransform(int index) { return InterpolateTransforms(g_Alpha, index); }
    inline std::vector<JPH::BodyID>& GetBodyIDs() { return body_ids; }

    inline void SetModelID(uint32_t id) { model_id = id; }
    inline void SetAnimated(bool anim) { animated = anim; }

    inline void SetPosition(glm::vec3 position, int index){ 
        transforms[index].second.SetPosition(position); 
        transforms[index].first.SetPosition(position);
        GetPhysicsSystem().GetBodyInterface().SetPosition(body_ids[index], GLMToJPH(position), JPH::EActivation::DontActivate);
    }

    inline void SetRotation(glm::vec3 rotation, int index){ 
        transforms[index].second.SetRotation(rotation); 
        transforms[index].first.SetRotation(rotation);
        GetPhysicsSystem().GetBodyInterface().SetRotation(body_ids[index], GLMToJPH(glm::quat(rotation)), JPH::EActivation::DontActivate);
    }

    inline void SetScale(glm::vec3 scale, int index){ 
        transforms[index].second.SetScale(scale); 
        transforms[index].first.SetScale(scale);
    }

private:
    uint32_t model_id = std::numeric_limits<uint32_t>::max();
    bool animated;
    std::vector<std::pair<GameObjectTransform, GameObjectTransform>> transforms;
    std::vector<JPH::BodyID> body_ids;
};