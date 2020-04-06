#ifndef __GAME_H__
#define __GAME_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

#include <map>
#include <string>

#include "maze.h"

#include "render/skybox.h"
#include "render/world.h"
#include "render/hole.h"
#include "render/teleport.h"
#include "render/particle.h"
#include "render/hud.h"

#include "game_logic.h"



static const glm::mat4 biasMatrix(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f);

class game {
private:
	maze *m;

	skyboxRenderer skybox;
	worldRenderer world;
	holeRenderer hole;
	teleportRenderer teleport;
	particleRenderer particle;
	hudRenderer hud;

	gameLogic logic;

	glm::mat4 m_view;
	glm::mat4 m_proj;
	glm::mat4 m_marble;
	glm::mat4 m_light;

	light sun;

public:
	game(maze *m);

public:
	bool init();

	void setStatus(char *str) {
		hud.setStatus(str);
	}

	void handleEvent(SDL_Event &e);

	void tick();
	void render();

	const light &worldLight() {
		return sun;
	}

	light viewLight();

	bool hasTeleported() {
		return logic.hasTeleported();
	}

	bool hasWon() {
		return logic.hasWon();
	}

	const glm::mat4 &viewMatrix() {
		return m_view;
	}

	const glm::mat4 &projectionMatrix() {
		return m_proj;
	}

	const glm::mat4 &marbleMatrix() {
		return m_marble;
	}

	glm::mat4 &lightMatrix() {
		return m_light;
	}
};

#endif // __GAME_H__

