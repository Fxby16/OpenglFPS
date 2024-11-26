#include <Serializer.hpp>
#include <ResourceManager.hpp>
#include <PredefinedMeshes.hpp>
#include <Physics.hpp>
#include <Model.hpp>

#include <glm.hpp>

#include <fstream>

void SerializeMap(const std::string& path)
{
    nlohmann::json j;

    auto& Models = GetResourceManager().GetGameObjects();
    for(auto& model : Models)
    {
        nlohmann::json model_json;
        Serialize(model_json, model.second);

        if(!model.second.IsAnimated()){
            j["models"].push_back(model_json);
        }else{
            j["skinnedModels"].push_back(model_json);
        }
    }

    std::ofstream file(path);
    file << j.dump(4);
    file.close();
}

void to_json(nlohmann::json& j, const glm::mat4& mat)
{
    j = nlohmann::json::array();
    for(int i = 0; i < 4; i++){
        nlohmann::json row = nlohmann::json::array();
        for(int j = 0; j < 4; j++){
            row.push_back(mat[i][j]);
        }
        j.push_back(row);
    }
}

void from_json(const nlohmann::json& j, glm::mat4& mat)
{
    for(int row = 0; row < 4; row++){
        for(int col = 0; col < 4; col++){
            mat[row][col] = j[row][col].get<float>();
        }
    }
}

void Serialize(nlohmann::json& j, GameObject& model)
{
    if(model.IsAnimated()){
        j["path"] = std::filesystem::relative(GetSkinnedModel(model.GetModelID())->model.GetPath(), std::filesystem::current_path()).string();
        j["id"] = model.GetModelID();
        j["name"] = GetSkinnedModel(model.GetModelID())->model.GetName();
        j["gamma_correction"] = GetSkinnedModel(model.GetModelID())->model.GetGammaCorrection();

        nlohmann::json animationsPaths = nlohmann::json::array();
        nlohmann::json animationsTicksPerSecond = nlohmann::json::array();

        for (const auto& animation : GetSkinnedModel(model.GetModelID())->animator.GetAnimationsInfo())
        {
            animationsPaths.push_back(animation.path);
            animationsTicksPerSecond.push_back(animation.ticksPerSecond);
        }
        j["animationsPaths"] = animationsPaths;
        j["animationsTicksPerSecond"] = animationsTicksPerSecond;

        nlohmann::json transforms = nlohmann::json::array();
        for (const auto& transform : model.GetTransforms())
        {
            nlohmann::json transform_json;
            to_json(transform_json, transform.second.mat);
            transforms.push_back(transform_json);
        }
        j["transforms"] = transforms;
    }else{
        j["path"] = std::filesystem::relative(GetModel(model.GetModelID())->GetName(), std::filesystem::current_path()).string();
        j["id"] = model.GetModelID();
        j["name"] = GetModel(model.GetModelID())->GetName();
        j["gamma_correction"] = GetModel(model.GetModelID())->GetGammaCorrection();

        nlohmann::json transforms = nlohmann::json::array();  // Create an empty JSON array
        for (const auto& transform : model.GetTransforms())
        {
            nlohmann::json transform_json;
            to_json(transform_json, transform.second.mat);
            transforms.push_back(transform_json);
        }
        j["transforms"] = transforms;
    }
}

void DeserializeMap(const std::string& path)
{
    std::ifstream file(path);

    if(!file.is_open()){
        return;
    }

    nlohmann::json j;
    file >> j;

    file.close();

    uint32_t model_id = std::numeric_limits<uint32_t>::max();

    for(auto& model_json : j["models"]){
        if(model_json["name"].get<std::string>().size() > 0){
            if(model_json["name"] == "CUBE"){
                model_id = GetGameObject(g_Cube)->GetModelID();
            }else if(model_json["name"] == "SPHERE"){
                model_id = GetGameObject(g_Sphere)->GetModelID();
            }
        }else{
            if(model_json.find("id") != model_json.end()){
                model_id = model_json["id"];
                LoadModel(model_id, model_json["path"], model_json["gamma_correction"]);
            }else{
                model_id = LoadModel((const std::string&) model_json["path"], model_json["gamma_correction"]);
            }
        }

        auto& model = *GetModel(model_id);
        uint32_t go = LoadGameObject(model_id, false);
        std::vector<Mesh>& meshes = model.GetMeshes();
        OBB model_obb;

        for(auto& mesh : meshes){
            model_obb.extents.x = std::max(model_obb.extents.x, mesh.GetAABB().max.x - mesh.GetAABB().min.x);
            model_obb.extents.y = std::max(model_obb.extents.y, mesh.GetAABB().max.y - mesh.GetAABB().min.y);
            model_obb.extents.z = std::max(model_obb.extents.z, mesh.GetAABB().max.z - mesh.GetAABB().min.z);
        }

        for(auto& transform_json : model_json["transforms"]){
            glm::mat4 transform;
            from_json(transform_json, transform);

            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec4 perspective;
            glm::vec3 skew;
            glm::decompose(transform, scale, rotation, translation, skew, perspective); 

            JPH::BodyID body = CreateBoxShape(model_obb.extents * scale, translation, rotation, Layers::NON_MOVING);
            GetGameObject(go)->AddInstance(GameObjectTransform(transform), body);
        }
    }

    for(auto& skinnedModel_json : j["skinnedModels"]){
        if(skinnedModel_json.find("id") != skinnedModel_json.end()){
            model_id = skinnedModel_json["id"];
            LoadSkinnedModel(model_id, skinnedModel_json["path"], skinnedModel_json["animationsPaths"][0], skinnedModel_json["animationsTicksPerSecond"][0], skinnedModel_json["gamma_correction"]);
        }else{
            model_id = LoadSkinnedModel((const std::string&) skinnedModel_json["path"], skinnedModel_json["animationsPaths"][0], skinnedModel_json["animationsTicksPerSecond"][0], skinnedModel_json["gamma_correction"]);
        }

        for(int i = 1; i < j["animationsPaths"].size(); i++){
            GetSkinnedModel(model_id)->AddAnimation(j["animationsPaths"][i], j["animationsTicksPerSecond"][i]);
        }

        auto& model = *GetSkinnedModel(model_id);
        uint32_t go = LoadGameObject(model_id, true);
        std::vector<Mesh>& meshes = model.model.GetMeshes();
        OBB model_obb;

        for(auto& mesh : meshes){
            model_obb.extents.x = std::max(model_obb.extents.x, mesh.GetAABB().max.x - mesh.GetAABB().min.x);
            model_obb.extents.y = std::max(model_obb.extents.y, mesh.GetAABB().max.y - mesh.GetAABB().min.y);
            model_obb.extents.z = std::max(model_obb.extents.z, mesh.GetAABB().max.z - mesh.GetAABB().min.z);
        }

        for(auto& transform_json : skinnedModel_json["transforms"]){
            glm::mat4 transform;
            from_json(transform_json, transform);

            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec4 perspective;
            glm::vec3 skew;
            glm::decompose(transform, scale, rotation, translation, skew, perspective); 

            JPH::BodyID body = CreateBoxShape(model_obb.extents * scale, translation, rotation, Layers::NON_MOVING);
            GetGameObject(go)->AddInstance(GameObjectTransform(transform), body);
        }
    }
}