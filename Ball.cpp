#include "Ball.hpp"
#include "PlayMode.hpp"
#include "Load.hpp"
#include "LitColorTextureProgram.hpp"
#include "common_consts.hpp"
#include "Platform.hpp"
#include "data_path.hpp"

#include <iostream>

extern Load< MeshBuffer > rhythm_parkour_meshes;

static constexpr float kRollingRotationSpeed = kRollingTranslationSpeed / Ball::kNormalRadius;

static constexpr float kJumpInitialSpeed = (0.7f * Platform::kPlatformUnitLen) * (-kGravity) / (kRollingTranslationSpeed);

Load< Sound::Sample > jump_sfx_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("JumpSound.wav"));
});

Ball::Ball(PlayMode* playmode) : 
drawable_(&rotation_transform_),
playmode_(playmode),
particle_system_(playmode->platform_manager.GetMainTransform()),
verticle_speed_(0.0f),
status_(Status::ROLLING)
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
            Sound::play(*jump_sfx_sample, 0.0f, 10.0f);
            verticle_speed_ = kJumpInitialSpeed;
        } else if (status_ == Status::CROUCHING) {
            translation_transform_.position[2] = kCrouchRadius - kNormalRadius;
            drawable_.pipeline.mesh = &(rhythm_parkour_meshes->lookup("PlayerCrouch"));
            playmode_->timer_manager.AddTimer(static_cast<long long>(1000.0f * (2 * kBeatInterval * kBeatPerPlatform)), [&]() {
                status_ = Status::ROLLING;
                translation_transform_.position[2] = 0.0f;
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

void Ball::Draw(Scene& scene) const
{
    particle_system_.Draw(scene);
    scene.dynamic_drawables.emplace_back(drawable_);
}

void Ball::Update(float elapsed)
{
    particle_system_.Update(elapsed, glm::vec3(0.0f, -Ball::kNormalRadius, 0.0f) + translation_transform_.position);
    Animate(elapsed);
}
