#include <Serializer.hpp>
#include <ResourceManager.hpp>
#include <PredefinedMeshes.hpp>

#include <glm.hpp>

#include <fstream>

void SerializeMap(const std::string& path)
{
    nlohmann::json j;

    auto& Models = GetResourceManager().GetModels();
    for(auto& model : Models)
    {
        nlohmann::json model_json;
        Serialize(model_json, model.second, model.first);
        j["models"].push_back(model_json);
    }

    auto& skinnedModels = GetResourceManager().GetSkinnedModels();
    for(auto& skinnedModel : skinnedModels)
    {
        nlohmann::json model_json;
        Serialize(model_json, skinnedModel.second, skinnedModel.first);
        j["skinnedModels"].push_back(model_json);
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

void Serialize(nlohmann::json& j, Model& model, uint32_t id)
{
    j["path"] = std::filesystem::relative(model.GetPath(), std::filesystem::current_path()).string();
    j["id"] = id;
    j["name"] = model.GetName();
    j["gamma_correction"] = model.GetGammaCorrection();

    nlohmann::json transforms = nlohmann::json::array();  // Create an empty JSON array
    for (const auto& transform : model.GetTransforms())
    {
        nlohmann::json transform_json;
        to_json(transform_json, transform);
        transforms.push_back(transform_json);
    }
    j["transforms"] = transforms;
}

void Serialize(nlohmann::json& j, SkinnedModel& model, uint32_t id)
{
    j["path"] = std::filesystem::relative(model.model.GetPath(), std::filesystem::current_path()).string();
    j["id"] = id;
    j["name"] = model.model.GetName();
    j["gamma_correction"] = model.model.GetGammaCorrection();

    nlohmann::json animationsPaths = nlohmann::json::array();
    nlohmann::json animationsTicksPerSecond = nlohmann::json::array();

    for (const auto& animation : model.animator.GetAnimationsInfo())
    {
        animationsPaths.push_back(animation.path);
        animationsTicksPerSecond.push_back(animation.ticksPerSecond);
    }
    j["animationsPaths"] = animationsPaths;
    j["animationsTicksPerSecond"] = animationsTicksPerSecond;

    nlohmann::json transforms = nlohmann::json::array();
    for (const auto& transform : model.model.GetTransforms())
    {
        nlohmann::json transform_json;
        to_json(transform_json, transform);
        transforms.push_back(transform_json);
    }
    j["transforms"] = transforms;
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
                model_id = g_Cube;
            }else if(model_json["name"] == "SPHERE"){
                model_id = g_Sphere;
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

        for(auto& transform_json : model_json["transforms"]){
            glm::mat4 transform;
            from_json(transform_json, transform);
            model.AddTransform(transform);
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

        for(auto& transform_json : skinnedModel_json["transforms"]){
            glm::mat4 transform;
            from_json(transform_json, transform);
            GetSkinnedModel(model_id)->model.AddTransform(transform);
        }
    }
}