#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Judge.hpp"
#include "common_consts.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <stdexcept>
#include <fstream>

Load< MeshBuffer > rhythm_parkour_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer *ret = new MeshBuffer(data_path("rhythm_parkour.pnct"));
	ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > rhythm_parkour_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("rhythm_parkour.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = rhythm_parkour_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.mesh = &mesh;
	});
});

Load< Sound::Sample > rhythm_parkour_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("rhythm_parkour.wav"));
});

PlayMode::PlayMode() : 
scene(*rhythm_parkour_scene),
overlay(this),
judge(this)
{
	ball = new Ball(this);

	// load map
	std::ifstream f(data_path("rhythm_parkour.map"));
    if (!f.is_open()) {
        throw std::runtime_error("Can not open map file");
    }

    int last_repeat_num = 0;
    int repeat_until;
    int type;

    while (f >> repeat_until >> type) {
        int to_repeat = repeat_until - last_repeat_num;
        if (to_repeat <= 0) {
            throw std::runtime_error("Wrong format of map file!");
        }
        
        while (to_repeat--) {
            platform_manager.AddPlatform(static_cast<Platform::Type>(type));
        }

		if (type == static_cast<int>(Platform::Type::OBSTACLE)) {
			judge.AddBeat(PlatformManager::GetPlatformTime(last_repeat_num));
		}

		last_repeat_num = repeat_until;
    }

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	background_music = Sound::play(*rhythm_parkour_sample, 0.0f, 10.0f);
	judge.StartTimer();
	platform_manager.StartMoving();
	overlay.AddText("Help", "Jump: Space Camera: Mouse", glm::vec3(0.1f, 0.1f, 0.0f));
	overlay.AddText("3", "3", glm::vec3(10.0f, 10.0f, 0.0f), 8500, 9000, kOrangeColor);
	overlay.AddText("2", "2", glm::vec3(10.0f, 10.0f, 0.0f), 9000, 9500, kOrangeColor);
	overlay.AddText("1", "1", glm::vec3(10.0f, 10.0f, 0.0f), 9500, 10000, kOrangeColor);
	timer_manager.AddTimer(79000, [&](){
		judge.AddFinalScoreText();
	});
}

PlayMode::~PlayMode() {
	delete ball;
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} if (evt.key.keysym.sym == SDLK_SPACE) {
			if (!jump.pressed) {
				jump_triggered = true;
			}
			jump.downs += 1;
			jump.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LCTRL) {
			if (!crouch.pressed) {
				crouch_triggered = true;
			}
			crouch.downs += 1;
			crouch.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_SPACE) {
			jump.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LCTRL) {
			crouch.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			float motion = -evt.motion.yrel / float(window_size.y);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(motion * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	timer_manager.Update();
	platform_manager.Update(elapsed);
	judge.Update(jump_triggered);

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 right = frame[0];
		glm::vec3 at = frame[3];
		Sound::listener.set_position_right(at, right, 1.0f / 60.0f);
	}

	if (jump_triggered) {
		ball->SetStatus(Ball::Status::JUMPING);
		jump_triggered = false;
	}

	if (crouch_triggered) {
		ball->SetStatus(Ball::Status::CROUCHING);
		crouch_triggered = false;
	}

	ball->Update(elapsed);
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	GL_ERRORS();
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	GL_ERRORS();
	glUseProgram(0);

	glClearColor(0.4f, 0.9f, 0.9f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	platform_manager.Draw(scene);
	ball->Draw(scene);
	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		float ofs = 2.0f / drawable_size.y;

		judge.AddScoreText();
		overlay.Draw(aspect, ofs);
	}
}
