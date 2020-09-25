#pragma once
#include "Scene.hpp"

struct PlayMode;

class Ball {
public:
    inline static constexpr float kNormalRadius { 0.5f };
    inline static constexpr float kCrouchRadius { 0.25f };

private:
    Scene::Transform translation_transform_;
    Scene::Transform rotation_transform_;
    Scene::Drawable drawable_;
    PlayMode* playmode_;

    float verticle_speed_;

    enum class Status : uint8_t {
        ROLLING = 0,
        JUMPING,
        CROUCHING
    } status_;

public:
    Ball(PlayMode* playmode);

    Scene::Drawable GetDrawable() const { return drawable_; }
    void SetStatus(Status status);
    void Animate(float elapsed_time);
};
