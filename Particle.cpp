#include "Particle.hpp"
#include "Load.hpp"
#include "LitColorTextureProgram.hpp"
#include "common_consts.hpp"
#include "Ball.hpp"

extern Load< MeshBuffer > rhythm_parkour_meshes;

ParticleSystem::ParticleSystem(const Scene::Transform* main_transform) :
main_transform_(main_transform)
{
    std::random_device rd;
    mt.seed(rd());

    drawable_template_.pipeline = lit_color_texture_program_pipeline;
    drawable_template_.pipeline.mesh = &(rhythm_parkour_meshes->lookup("Particle"));
}

void ParticleSystem::Emit(const glm::vec3& emit_point)
{
    Scene::Transform particle_transform;
    particle_transform.parent = main_transform_;
    particle_transform.position = main_transform_->make_world_to_local() * glm::vec4(emit_point, 1.0f);

    particle_transform.position[0] += std::uniform_real_distribution<float>(-Ball::kNormalRadius, Ball::kNormalRadius)(mt);
    particle_transform.position[2] += std::uniform_real_distribution<float>(0.5f, 1.5f)(mt);
    
    float scale_begin = std::uniform_real_distribution<float>(0.6f, 1.0f)(mt);
    float scale_end = std::uniform_real_distribution<float>(0.1f, 0.5f)(mt);

    particle_transform.scale = glm::vec3(scale_begin, scale_begin, scale_begin);

    float time_remain = std::uniform_real_distribution<float>(1.0f, 5.0f)(mt);
    float size_delta = (scale_end - scale_begin) / time_remain;

    Particle particle { particle_transform, size_delta, time_remain };

    particles_.emplace_back(particle);
}

void ParticleSystem::Update(float elapsed, const glm::vec3& emit_point)
{
    auto it = particles_.begin();

    while (it != particles_.end()) {
        Particle& particle = *it;
        particle.time_remain_ -= elapsed;
        if (particle.time_remain_ < 0) {
            auto tmp = it;
            ++it;
            particles_.erase(tmp);
            continue;
        }
        float scale = particle.transform_.scale[0] + particle.size_delta_ * elapsed;
        particle.transform_.scale = glm::vec3(scale, scale, scale);
        ++it;
    }

    static float time_since_last_emit { 0.0f };
    time_since_last_emit += elapsed;

    static float kEmitInterval = kBeatInterval / 4.0f;

    while (time_since_last_emit > kEmitInterval) {
        Emit(emit_point);
        Emit(emit_point);
        time_since_last_emit -= kEmitInterval;
    }
}

void ParticleSystem::Draw(Scene& scene) const
{
    for (const auto& particle : particles_) {
        drawable_template_.transform = &(particle.transform_);
        scene.dynamic_drawables.emplace_back(drawable_template_);
    }
}
