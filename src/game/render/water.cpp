#include "water.h"

#include "../game.h"
#include "../options.h"
#include "../resources.h"

#include "maze.h"

water::water(engine_options *options, game *m_game) : m_game(m_game),
	m_shader("water", SHADER_RESOURCE(s_water_v), SHADER_RESOURCE(s_water_f)),
	refraction(options->window_width, options->window_height),
	refractionDepth(options->window_width, options->window_height, true)
{
	m_shader.add_uniform("projectionMatrix", &m_game->m_proj);
	m_shader.add_uniform("viewMatrix", &m_game->m_view);
	m_shader.add_uniform("cameraPosition", &m_game->m_camera.position);
	m_shader.add_uniform("lightDirection", &m_game->sun.direction);
	m_shader.add_uniform("refractionTexture", &refractionSampler);
	m_shader.add_uniform("dudvTexture", &dudvSampler);
	m_shader.add_uniform("normalTexture", &normalSampler);
	m_shader.add_uniform("globalTime", &globalTime);
	m_shader.add_uniform("shininess", &shininess);
	
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