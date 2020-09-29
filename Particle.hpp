#pragma once

#include "Scene.hpp"

#include <vector>
#include <random>

class ParticleSystem {
private:
    struct Particle {
        Scene::Transform transform_;

        float size_delta_;

        float time_remain_;
    };

    std::vector<Particle> particles_;
    mutable Scene::Drawable drawable_template_;
    const Scene::Transform* main_transform_;
    std::mt19937 mt;

public:
    ParticleSystem(const Scene::Transform* main_transform);
    void Emit(const glm::vec3& emit_point);
    void Update(float elapsed, const glm::vec3& emit_point);
    void Draw(Scene& scene) const;
};
