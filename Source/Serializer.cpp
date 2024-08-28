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
        Serialize(model_json, model.second);
        j["Models"].push_back(model_json);
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

void Serialize(nlohmann::json& j, Model& model)
{
    j["path"] = model.GetPath();
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
    for(auto& model_json : j["models"])
    {
        if(model_json["name"].get<std::string>().size() > 0){
            if(model_json["name"] == "CUBE"){
                model_id = g_Cube;
            }else if(model_json["name"] == "SPHERE"){
                model_id = g_Sphere;
            }
        }else{
            model_id = LoadModel(model_json["path"], model_json["gamma_correction"]);
        }

        auto& model = GetModel(model_id);

        for(auto& transform_json : model_json["transforms"])
        {
            glm::mat4 transform;
            from_json(transform_json, transform);
            model.AddTransform(transform);
        }
    }
}