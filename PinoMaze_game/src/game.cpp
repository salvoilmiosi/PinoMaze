#include "game.h"

#include "res_loader.h"
#include "globals.h"

#include <glm/gtc/matrix_transform.hpp>

game::game(maze *m) : m(m), world(m), hole(m), teleport(m), logic(m) {}

bool game::init() {
	loadMaterials();

	if (!skybox.init()) return false;
	if (!world.init()) return false;
	if (!hole.init()) return false;
	if (!teleport.init()) return false;
	if (!particle.init()) return false;
	if (!hud.init()) return false;

	if (!logic.init()) return false;

    m_proj = glm::perspective(glm::radians(90.f), (float)windowWidth / (float)windowHeight, 0.1f, skyboxSize * 2.f);

	sun.direction = glm::vec3(0.43555f, 0.5f, -0.25391f);

    return true;
}

void game::handleEvent(SDL_Event &e) {
	logic.handleEvent(e);
}

void game::tick() {
    logic.tick();

	m_view = logic.getCamera().viewMatrix();
	m_marble = logic.marbleMatrix();

	world.tick(this);
	hole.tick(this);
	particle.tick(this);
}

void game::render() {
	glClear(GL_DEPTH_BUFFER_BIT);

    skybox.render(this);
	world.render(this);
	teleport.render(this);

	hole.bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT);
	skybox.render(this);
	world.renderRefraction(this);

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

#define LOAD_TEXTURE(RES_ID) make_shared<texture>(loadImageFromResource(RES_ID))

void game::loadMaterials() {
	auto TEX_MARBLE_TEXTURE = LOAD_TEXTURE("IDT_MARBLE_TEXTURE");
	auto TEX_MARBLE_NORMALS = LOAD_TEXTURE("IDT_MARBLE_NORMALS");
	auto TEX_FLOOR_TEXTURE = LOAD_TEXTURE("IDT_FLOOR_TEXTURE");
	auto TEX_FLOOR_NORMALS = LOAD_TEXTURE("IDT_FLOOR_NORMALS");
	auto TEX_BRICKS_TEXTURE = LOAD_TEXTURE("IDT_BRICKS_TEXTURE");
	auto TEX_BRICKS_NORMALS = LOAD_TEXTURE("IDT_BRICKS_NORMALS");
	auto TEX_COBBLE_TEXTURE = LOAD_TEXTURE("IDT_COBBLE_TEXTURE");
	auto TEX_COBBLE_NORMALS = LOAD_TEXTURE("IDT_COBBLE_NORMALS");
	auto TEX_TILES_TEXTURE = LOAD_TEXTURE("IDT_TILES_TEXTURE");
	auto TEX_TILES_NORMALS = LOAD_TEXTURE("IDT_TILES_NORMALS");
	auto TEX_PLASTER_TEXTURE = LOAD_TEXTURE("IDT_PLASTER_TEXTURE");
	auto TEX_PLASTER_NORMALS = LOAD_TEXTURE("IDT_PLASTER_NORMALS");
	auto TEX_ARROW_TEXTURE = LOAD_TEXTURE("IDT_ARROW_TEXTURE");
	auto TEX_RUST_TEXTURE = LOAD_TEXTURE("IDT_RUST_TEXTURE");
	auto TEX_RUST_NORMALS = LOAD_TEXTURE("IDT_RUST_NORMALS");

	MAT_FLOOR.specular = 0x262626;
	MAT_FLOOR.tex = TEX_FLOOR_TEXTURE;
	MAT_FLOOR.normals = TEX_FLOOR_NORMALS;

	MAT_FLOOR_REFRACTED.diffuse = 0x9999b2;
	MAT_FLOOR_REFRACTED.specular = 0x000000;
	MAT_FLOOR_REFRACTED.tex = TEX_FLOOR_TEXTURE;

	MAT_BRICKS.tex = TEX_BRICKS_TEXTURE;
	MAT_BRICKS.normals = TEX_BRICKS_NORMALS;

	MAT_COBBLE.tex = TEX_COBBLE_TEXTURE;
	MAT_COBBLE.normals = TEX_COBBLE_NORMALS;

	MAT_TILES.diffuse = 0xffffbf;
	MAT_TILES.specular = 0x666666;
	MAT_TILES.tex = TEX_TILES_TEXTURE;
	MAT_TILES.normals = TEX_TILES_NORMALS;

	MAT_PLASTER.ambient = 0xffffff;
	MAT_PLASTER.diffuse = 0xaea4a9;
	MAT_PLASTER.specular = 0x333333;
	MAT_PLASTER.tex = TEX_PLASTER_TEXTURE;
	MAT_PLASTER.normals = TEX_PLASTER_NORMALS;

	MAT_MARBLE.ambient = 0xb2b2b2;
	MAT_MARBLE.specular = 0x999999;
	MAT_MARBLE.tex = TEX_MARBLE_TEXTURE;
	MAT_MARBLE.normals = TEX_MARBLE_NORMALS;

	MAT_MARBLE_REFRACTED.diffuse = 0x9999b2;
	MAT_MARBLE_REFRACTED.specular = 0x000000;
	MAT_MARBLE_REFRACTED.tex = TEX_MARBLE_TEXTURE;

	MAT_START.diffuse = 0xff0000;
	MAT_START.tex = TEX_PLASTER_TEXTURE;
	MAT_START.normals = TEX_PLASTER_NORMALS;

	MAT_END.diffuse = 0xff00ff;
	MAT_END.tex = TEX_PLASTER_TEXTURE;
	MAT_END.normals = TEX_PLASTER_NORMALS;

	MAT_ARROW.tex = TEX_ARROW_TEXTURE;
	MAT_ARROW.normals = TEX_PLASTER_NORMALS;

	MAT_RUST.tex = TEX_RUST_TEXTURE;
	MAT_RUST.normals = TEX_RUST_NORMALS;
}