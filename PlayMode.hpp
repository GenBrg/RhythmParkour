#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "Ball.hpp"
#include "Timer.hpp"
#include "Platform.hpp"
#include "Overlay.hpp"
#include "Judge.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, jump, crouch;

	bool jump_triggered { false };
	bool crouch_triggered { false };

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	Ball* ball;

	TimerManager timer_manager;
	PlatformManager platform_manager;
	Overlay overlay;
	Judge judge;

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > background_music;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
