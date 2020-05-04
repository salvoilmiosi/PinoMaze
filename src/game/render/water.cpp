#include "water.h"

#include "../game.h"
#include "../options.h"
#include "../resources.h"

#include "maze.h"

water::water(context *con, game *m_game) : m_game(m_game),
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

	refraction(con->window_width, con->window_height),
	refractionDepth(con->window_width, con->window_height, true)
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

	maze *m = m_game->m_maze;

    for (int y = gridy; y < MIN(gridy + gridsize, m->height()); ++y) {
        for (int x = gridx; x < MIN(gridx + gridsize, m->width()); ++x) {
			tile *t = m->getTile(x, y);
			if (t->state == STATE_BLOCK) {
				matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3(x * tileSize, -blockHeight, y * tileSize)));
			}
		}
    }

	vao.update_matrices(1, matrices.data(), matrices.size(), 1, true);
}

void water::render(float deltaNano) {
	refractionSampler.bind(&refraction);
	dudvSampler.bind(getTexture("TEX_WATER_DUDV").get());
	normalSampler.bind(getTexture("TEX_WATER_NORMALS").get());

	globalTime += deltaNano / 1000000.f;
	m_shader.use();

	vao.draw_instances();
}