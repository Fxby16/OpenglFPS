#pragma once

#include <raylib.h>
#include <string>
#include <vector>

class ModelEx{
public:
    ModelEx() = default;
    ~ModelEx() = default;

    void Load(const std::string& path);
    void Unload();

    inline BoundingBox& GetMeshBB(int index) { return m_MeshesBB[index]; }
    inline Model& GetModel() { return m_Model; }

private:
    Model m_Model;
    std::vector<BoundingBox> m_MeshesBB;
};