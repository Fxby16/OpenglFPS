#include <GameObject.hpp>
#include <ResourceManager.hpp>
#include <Physics.hpp>

GameObject::GameObject(uint32_t model_id, bool animated)
    : model_id(model_id), animated(animated){}

void GameObject::AddInstance(GameObjectTransform transform, JPH::BodyID body_id)
{
    transforms.push_back({transform, transform});
    body_ids.push_back(body_id);
    LogMessage("Adding body with id: %u", body_id.GetIndexAndSequenceNumber());
    GetPhysicsSystem().GetBodyInterface().AddBody(body_id, JPH::EActivation::DontActivate);
}

void GameObject::RemoveInstance(int index)
{
    GetPhysicsSystem().GetBodyInterface().RemoveBody(body_ids[index]);
    GetPhysicsSystem().GetBodyInterface().DestroyBody(body_ids[index]);
    body_ids.erase(body_ids.begin() + index);
    transforms.erase(transforms.begin() + index);
}

void GameObject::Update()
{
    for(int i = 0; i < body_ids.size(); i++){
        auto& [previous, current] = transforms[i];

        //get physics body info and update current transform
        previous = current;
        current.pos = JPHToGLM(GetPhysicsSystem().GetBodyInterface().GetPosition(body_ids[i]));
        current.rot = JPHToGLM(GetPhysicsSystem().GetBodyInterface().GetRotation(body_ids[i]));
        
        current.mat = current.GetMatrix();  
    }
}

glm::mat4 GameObject::InterpolateTransforms(float alpha, int index)
{
    return GameObjectTransform::Interpolate(transforms[index].first, transforms[index].second, alpha).mat;
}

void GameObject::Draw(Shader& shader)
{
    for(auto& [previous, current] : transforms){
        if(animated){
            SkinnedModel* skinned_model = GetSkinnedModel(model_id);
            if(skinned_model){
                skinned_model->animator.UploadFinalBoneMatrices(shader);
                skinned_model->model.Draw(shader, GetCamera().GetViewMatrix(), GameObjectTransform::Interpolate(previous, current, g_Alpha).mat);
            }
        }else{
            Model* model = GetModel(model_id);
            if(model){
                model->Draw(shader, GetCamera().GetViewMatrix(), GameObjectTransform::Interpolate(previous, current, g_Alpha).mat);
            }
        }
    }
}

void GameObject::DrawShadow(Shader& shader, glm::mat4 light_space_matrix)
{
    for(auto& [previous, current] : transforms){
        if(animated){
            SkinnedModel* skinned_model = GetSkinnedModel(model_id);
            if(skinned_model){
                skinned_model->animator.UploadFinalBoneMatrices(shader);
                skinned_model->model.DrawShadows(shader, light_space_matrix, GameObjectTransform::Interpolate(previous, current, g_Alpha).mat);
            }
        }else{
            Model* model = GetModel(model_id);
            if(model){
                model->DrawShadows(shader, light_space_matrix, GameObjectTransform::Interpolate(previous, current, g_Alpha).mat);
            }
        }
    }
}