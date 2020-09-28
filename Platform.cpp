#include "Platform.hpp"
#include "LitColorTextureProgram.hpp"
#include "Load.hpp"
#include "common_consts.hpp"
#include "Ball.hpp"

#include <stdexcept>
#include <iostream>

extern Load< MeshBuffer > rhythm_parkour_meshes;

constexpr const char* const kMeshNames[] {
    "Slope",
    "Platform",
    "Obstacle"
};

void PlatformManager::AddPlatform(Platform::Type type)
{
    Platform* last_platform = nullptr;

    if (!platforms_.empty()) {
        last_platform = &(platforms_.back());
    }

    platforms_.emplace_back(type, &main_transform_, last_platform);
}

Platform::Platform(Type type, Scene::Transform* main_transform, const Platform* last_platform) :
type_(type),
drawable_(&transform_)
{
    drawable_.pipeline = lit_color_texture_program_pipeline;
    drawable_.pipeline.mesh = &(rhythm_parkour_meshes->lookup(kMeshNames[static_cast<uint8_t>(type)]));

    transform_.parent = main_transform;

    float last_y = 0.0f;
    float last_z = 0.0f;

    transform_.position[1] = kPlatformUnitLen;

    if (last_platform) {
        last_y = last_platform->transform_.position[1];
        last_z = last_platform->transform_.position[2];

        switch(last_platform->type_) {
            case Type::SLOPE_DOWN:
                transform_.position[2] = -kPlatformUnitLen;
            break;
            default:;
        }
    }

    transform_.position[1] += last_y;
    transform_.position[2] += last_z;

    switch (type) {
        case Type::PLAIN:
        case Type::OBSTACLE:
        speed_vec_[1] = -kRollingTranslationSpeed;
        break;
        case Type::SLOPE_DOWN:
        speed_vec_[1] = -kRollingTranslationSpeed;
        speed_vec_[2] = kRollingTranslationSpeed;
        break;
    }
}

void PlatformManager::Update(float elapsed)
{
    while (true) {
        if (cur_platform_idx_ >= platforms_.size()) {
            return;
        }

        Platform& current_platform = platforms_[cur_platform_idx_];
        Platform* next_platform = (cur_platform_idx_ + 1 >= platforms_.size()) ? nullptr : &platforms_[cur_platform_idx_ + 1];

        glm::vec3 speed_vec = current_platform.GetSpeedVec();
        glm::vec3 delta_s = elapsed * speed_vec;

        glm::vec3 platform_world_pos = current_platform.GetWorldPos();
        glm::vec3 platform_world_pos_to_be = platform_world_pos + delta_s;
        float platform_dist = current_platform.GetPlatformDist(next_platform);

        if (-platform_world_pos_to_be[1] > platform_dist) {
            float time_to_travel_to_next_platform = (platform_dist + platform_world_pos[1]) / (-speed_vec[1]);
            main_transform_.position += time_to_travel_to_next_platform * speed_vec;
            elapsed -= time_to_travel_to_next_platform;
            ++cur_platform_idx_;
        } else {
            main_transform_.position += delta_s;
            return;
        }
    }
}

void Platform::Draw(Scene& scene) const
{
    scene.dynamic_drawables.emplace_back(drawable_);
}

void PlatformManager::Draw(Scene& scene) const
{
    for (const auto& platform : platforms_) {
        platform.Draw(scene);
    }
}

PlatformManager::PlatformManager(const std::string& filename)
{
    main_transform_.position[1] = -Platform::kPlatformUnitLen;
    main_transform_.position[2] = -Ball::kNormalRadius - (Platform::kPlatformUnitLen / 2.0f);
}

float Platform::GetPlatformDist(const Platform* next_platform) const
{
    if (type_ == Type::PLAIN || type_ == Type::OBSTACLE) {
        if (next_platform && next_platform->type_ == Type::SLOPE_DOWN) {
            return kPlatformUnitLen / 2.0f + glm::tan(glm::radians(45.0f / 2.0f)) * Ball::kNormalRadius;
        }
        return kPlatformUnitLen / 2.0f;
    }
    
    if (type_ == Type::SLOPE_DOWN) {
        if (!next_platform) {
            return kPlatformUnitLen / 2.0f;
        }

        Type next_type = next_platform->type_;
        if (next_type == Type::PLAIN || next_type == Type::OBSTACLE) {
            return kPlatformUnitLen / 2.0f + glm::tan(glm::radians(45.0f / 2.0f)) * Ball::kNormalRadius;
        } else if (next_type == Type::SLOPE_DOWN) {
            return kPlatformUnitLen / 2.0f + glm::sin(glm::radians(45.0f)) * Ball::kNormalRadius;
        }
    }

    throw std::runtime_error("Unsupported type");
}
