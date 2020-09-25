#include "Ball.hpp"
#include "PlayMode.hpp"
#include "Load.hpp"
#include "LitColorTextureProgram.hpp"

extern Load< MeshBuffer > rhythm_parkour_meshes;

// static constexpr float kRotationSpeed = glm::radians(3);
static constexpr float kGravity = -9.8f;

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
            verticle_speed_ = 20.0f;
        } else if (status_ == Status::CROUCHING) {
            translation_transform_.position[2] = kCrouchRadius - kNormalRadius;
            playmode_->timer_manager.AddTimer(500, [&]() {
                status_ = Status::ROLLING;
                translation_transform_.position[2] = 0;
            });
        }
    }
}

void Ball::Animate(float elapsed)
{
    // Rolling
    

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
