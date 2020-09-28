#pragma once
#include "Platform.hpp"

inline constexpr float kGravity = -500.0f;
inline constexpr float kBPM = 120.0f;
inline constexpr float kBeatInterval = 60.0f / kBPM;
inline constexpr float kBeatPerPlatform = 1.0f / 4.0f;
inline constexpr float kRollingTranslationSpeed = Platform::kPlatformUnitLen / (kBeatInterval * kBeatPerPlatform);