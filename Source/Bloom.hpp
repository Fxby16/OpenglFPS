#pragma once

inline constexpr int NUM_MIPS = 7;

extern void InitBloom();
extern void DeinitBloom();
extern void BloomPass();

extern unsigned int GetBloomTexture();
extern void SetBloomThreshold(float threshold);
extern float GetBloomThreshold();
extern void SetBloomKnee(float knee);
extern float GetBloomKnee();