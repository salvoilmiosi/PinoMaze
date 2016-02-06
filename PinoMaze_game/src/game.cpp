#include "game.h"

#include "res_loader.h"
#include "globals.h"

#include <SDL2/SDL_mixer.h>
#include <glm/gtc/matrix_transform.hpp>

game::~game() {
    clean();
}

void game::setMaze(maze *_m) {
    m = _m;

    world.setMaze(m);
    hole.setMaze(m);
	teleport.setMaze(m);

    logic.setMaze(m);
}

bool game::init() {
	material::loadMaterials();

	if (!skybox.init()) return false;
	if (!world.init()) return false;
	if (!hole.init()) return false;
	if (!teleport.init()) return false;
	if (!particle.init()) return false;
	if (!hud.init()) return false;

    m_proj = glm::perspective(glm::radians(90.f), (float)windowWidth / (float)windowHeight, 0.1f, skyboxSize * sqrtf(2.f) * 1.2f);

    sun.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
    sun.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    sun.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	sun.direction = glm::vec3(0.43555f, 0.5f, -0.25391f);

    return true;
}

void game::clean() {
	skybox.clean();
    world.clean();
    hole.clean();
	teleport.clean();
	hud.clean();

	material::cleanMaterials();
}

void game::handleEvent(SDL_Event &e) {
	logic.handleEvent(e);
}

void game::tick() {
    logic.tick();
	m_view = logic.viewMatrix();
	m_marble = logic.marbleMatrix();

	world.tick(this);
	hole.tick(this);
	particle.tick(this);
	logic.resetTeleported();
}

void game::render() {
	glClear(GL_DEPTH_BUFFER_BIT);

    skybox.render(this);
	world.render(this);
	teleport.render(this);

	hole.bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT);
	skybox.render(this);
	world.renderRefraction();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	hole.render(this);
	particle.render(this);

	hud.render();
}

light game::viewLight() {
	light l = sun;
	l.direction = glm::vec3(m_view * glm::vec4(sun.direction, 0.0));
	return l;
}