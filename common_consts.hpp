#pragma once

#include "Platform.hpp"

inline constexpr float kGravity = -500.0f;
inline constexpr float kBPM = 120.0f;
inline constexpr float kBeatInterval = 60.0f / kBPM;
inline constexpr float kBeatPerPlatform = 1.0f / 4.0f;
inline constexpr float kRollingTranslationSpeed = Platform::kPlatformUnitLen / (kBeatInterval * kBeatPerPlatform);

// Colors
inline constexpr glm::u8vec4 kOrangeColor { 0xc4, 0xa0, 0x00, 0xff };
inline constexpr glm::u8vec4 kRedColor { 0xed, 0x21, 0x12, 0xff };
inline constexpr glm::u8vec4 kGreyColor { 0x60, 0x57, 0x57, 0xff };
inline constexpr glm::u8vec4 kGreenColor { 0x0d, 0xfc, 0x05, 0xff };
inline constexpr glm::u8vec4 kPurpleColor { 0x85, 0x00, 0xf2, 0xff };