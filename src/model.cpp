#include <model.hpp>

void ModelEx::Load(const std::string& path)
{
    m_Model = LoadModel(path.c_str());
    m_MeshesBB.resize(m_Model.meshCount);
    for(int i = 0; i < m_Model.meshCount; i++)
    {
        m_MeshesBB[i] = GetMeshBoundingBox(m_Model.meshes[i]);
    }
}

void ModelEx::Unload()
{
    m_MeshesBB.clear();
    UnloadModel(m_Model);
}