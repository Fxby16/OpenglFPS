#pragma once

#include <string>

#include <json.hpp>

class Model;
class SkinnedModel;
class GameObject;

extern void SerializeMap(const std::string& filename);
extern void DeserializeMap(const std::string& filename);

extern void Serialize(nlohmann::json& j, GameObject& model);