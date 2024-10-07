#pragma once

#include <string>

enum ToneMapping{
    ACES,
    UNCHARTED,
    FILMIC,
    REINHARD
};

extern void InitPostProcessing();
extern void PostProcessingPass();

extern void SetExposure(float exposure);
extern float GetExposure();
extern void SetBloomStrength(float intensity);
extern float GetBloomStrength();
extern void UseBloom(bool use);
extern bool GetUseBloom();
extern void SetToneMapping(int mapping);
extern int GetToneMapping();
extern void SetDirtTexture(const std::string& path);