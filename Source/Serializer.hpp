#pragma once

#include <string>

#include <json.hpp>

class Model;
class SkinnedModel;

extern void SerializeMap(const std::string& filename);
extern void DeserializeMap(const std::string& filename);

extern void Serialize(nlohmann::json& j, Model& model, uint32_t id);
extern void Serialize(nlohmann::json& j, SkinnedModel& model, uint32_t id);