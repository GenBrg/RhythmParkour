#pragma once

#include "Scene.hpp"

#include <deque>

class Platform
{
public:
    enum class Type : uint8_t {
        SLOPE_DOWN = 0,
        PLAIN,
        OBSTACLE,
        NONE
    };

    inline static constexpr float kPlatformUnitLen = 2.0f;

private:
    Scene::Transform transform_;
    Scene::Drawable drawable_;
    Type type_;
    glm::vec3 speed_vec_ { 0.0f };

public:
    ~Platform() {}
    Platform(Type type, Scene::Transform* main_transform, const Platform* last_platform);
    void Draw(Scene& scene) const;
    float GetPlatformDist(const Platform* next_platform) const;
    glm::vec3 GetWorldPos() const { return transform_.make_local_to_world()[3]; }
    glm::vec3 GetSpeedVec() const {return speed_vec_; };
};

class PlatformManager {
private:
    std::deque<Platform> platforms_;
    Scene::Transform main_transform_;
    size_t cur_platform_idx_ { 0 };
    
public:
    PlatformManager(const std::string& filename);
    ~PlatformManager() {}

    void AddPlatform(Platform::Type type);
    void Update(float elapsed);
    void Draw(Scene& scene) const;
};
