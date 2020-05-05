#include "water.h"

#include "../game.h"
#include "../options.h"
#include "../resources.h"

#include "maze.h"

water::water(engine *m_engine, game *m_game) : m_game(m_game), options(m_engine->options),
	m_shader("water", SHADER_RESOURCE(s_water_v), SHADER_RESOURCE(s_water_f),
		"projectionMatrix", &m_game->m_proj,
		"viewMatrix", &m_game->m_view,
		"cameraPosition", &m_game->m_camera.position,
		"lightDirection", &m_game->sun.direction,
		"refractionTexture", &refractionSampler,
		"dudvTexture", &dudvSampler,
		"normalTexture", &normalSampler,
		"globalTime", &globalTime,
		"shininess", &shininess),

	refraction(options.window_width, options.window_height),
	refractionDepth(options.window_width, options.window_height, true)
{
	refraction.setFilter(GL_NEAREST);
	refraction.setWrapParam(GL_CLAMP_TO_EDGE);

	refractionDepth.setFilter(GL_NEAREST);
	refractionDepth.setWrapParam(GL_CLAMP_TO_EDGE);

	refractionFBO.attachTexture(refraction);
	refractionFBO.attachDepthMap(refractionDepth);

    const glm::vec3 vertices[] = {
        glm::vec3(0.0, 0.f, 0.0),
        glm::vec3(0.0, 0.f, tileSize),
        glm::vec3(tileSize, 0.f, 0.0),
        glm::vec3(tileSize, 0.f, tileSize),
    };

	vao.update_vertices(0, vertices, sizeof(vertices) / sizeof(glm::vec3), {{0, ATTR_VEC3}});

	checkGlError("Failed to init hole model");
}

void water::load_models(int gridx, int gridy, int gridsize) {
    std::vector<glm::mat4> matrices;

	maze &m_maze = m_game->m_maze;

    for (int y = gridy; y < MIN(gridy + gridsize, m_maze.height()); ++y) {
        for (int x = gridx; x < MIN(gridx + gridsize, m_maze.width()); ++x) {
			tile *t = m_maze.getTile(x, y);
			if (t->state == STATE_BLOCK) {
				matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3(x * tileSize, -blockHeight, y * tileSize)));
			}
		}
    }

	vao.update_matrices(1, matrices.data(), matrices.size(), 1, true);
}

void water::render(float deltaNano) {
	refraction.bindTo(refractionSampler);
	materials::bindTexture(dudvSampler, "TEX_WATER_DUDV");
	materials::bindTexture(normalSampler, "TEX_WATER_NORMALS");

	globalTime += deltaNano / 1000000.f;
	m_shader.use();

	vao.draw_instances();
}