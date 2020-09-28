#pragma once
#include "Platform.hpp"

inline constexpr float kGravity = -30.0f;
inline constexpr float kBPM = 120.0f;
inline constexpr float kBeatInterval = 60.0f / kBPM;
inline constexpr float kBeatPerPlatform = 10.0f;
inline constexpr float kRollingTranslationSpeed = Platform::kPlatformUnitLen / (kBeatInterval * kBeatPerPlatform);