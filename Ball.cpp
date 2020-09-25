#include "Ball.hpp"
#include "PlayMode.hpp"
#include "Load.hpp"
#include "LitColorTextureProgram.hpp"

#include <iostream>

extern Load< MeshBuffer > rhythm_parkour_meshes;

static constexpr float kGravity = -500.0f;

static constexpr float kBPM = 120.0f;
static constexpr float kBeatInterval = 60.0f / kBPM;
static constexpr float kBeatPerPlatform = 1.0f / 6.0f;
static constexpr float kPlatformUnitLen = 2.0f;
static constexpr float kRollingTranslationSpeed = kPlatformUnitLen / (kBeatInterval * kBeatPerPlatform);
static constexpr float kRollingRotationSpeed = kRollingTranslationSpeed / Ball::kNormalRadius;

static constexpr float kJumpInitialSpeed = kPlatformUnitLen * (-kGravity) / (kRollingTranslationSpeed);

Ball::Ball(PlayMode* playmode) : 
drawable_(&rotation_transform_),
verticle_speed_(0.0f),
playmode_(playmode)
{
    rotation_transform_.parent = &translation_transform_;
    drawable_.pipeline = lit_color_texture_program_pipeline;
    drawable_.pipeline.mesh = &(rhythm_parkour_meshes->lookup("Player"));
}

void Ball::SetStatus(Status status)
{
    if (status_ == Status::ROLLING) {
        status_ = status;
        if (status_ == Status::JUMPING) {
            verticle_speed_ = kJumpInitialSpeed;
        } else if (status_ == Status::CROUCHING) {
            translation_transform_.position[2] = kCrouchRadius - kNormalRadius;
            drawable_.pipeline.mesh = &(rhythm_parkour_meshes->lookup("PlayerCrouch"));
            playmode_->timer_manager.AddTimer(static_cast<long long>(1000.0f * (2 * kBeatInterval * kBeatPerPlatform)), [&]() {
                status_ = Status::ROLLING;
                translation_transform_.position[2] = 0;
                drawable_.pipeline.mesh = &(rhythm_parkour_meshes->lookup("Player"));
            });
        }
    }
}

void Ball::Animate(float elapsed)
{
    // Rolling
    rotation_transform_.rotation *= glm::angleAxis(kRollingRotationSpeed * elapsed, glm::vec3(-1.0f, 0.0f, 0.0f));

    if (status_ == Status::JUMPING) {
        float& z = translation_transform_.position[2];
        z += verticle_speed_ * elapsed;
        verticle_speed_ += kGravity * elapsed;
        if (z <= 0.0f) {
            z = 0.0f;
            status_ = Status::ROLLING;
            verticle_speed_ = 0.0f;
        }
    }
}
