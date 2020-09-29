#pragma once
#include "Scene.hpp"
#include "Particle.hpp"

struct PlayMode;

class Ball {
public:
    inline static constexpr float kNormalRadius { 0.5f };
    inline static constexpr float kCrouchRadius { 0.25f };

    enum class Status : uint8_t {
        ROLLING = 0,
        JUMPING,
        CROUCHING
    };

private:
    Scene::Transform translation_transform_;
    Scene::Transform rotation_transform_;
    Scene::Drawable drawable_;
    PlayMode* playmode_;
    ParticleSystem particle_system_;

    float verticle_speed_;

    Status status_;

public:
    Ball(PlayMode* playmode);

    void Draw(Scene& scene) const;
    void SetStatus(Status status);
    void Animate(float elapsed);
    void Update(float elapsed);
};
