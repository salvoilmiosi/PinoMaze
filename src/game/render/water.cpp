#include "water.h"

#include "../game.h"
#include "../options.h"
#include "../resources.h"

#include "maze.h"

water::water(context *con, game *m_game) :
	m_shader("water", SHADER_RESOURCE(s_water_v), SHADER_RESOURCE(s_water_f)),
	refraction(con->window_width, con->window_height),
	refractionDepth(con->window_width, con->window_height, true)
{
	m_shader.add_uniform("projectionMatrix", &m_game->m_proj);
	m_shader.add_uniform("viewMatrix", &m_game->m_view);
	m_shader.add_uniform("cameraPosition", &m_game->m_camera.position);
	m_shader.add_uniform("lightDirection", &m_game->sun.direction);
	m_shader.add_uniform("refractionTexture", &refractionSampler.gl_samplerid);
	m_shader.add_uniform("dudvTexture", &dudvSampler.gl_samplerid);
	m_shader.add_uniform("normalTexture", &normalSampler.gl_samplerid);
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

	vao.update_vertices(0, vertices, sizeof(vertices), {{0, ATTR_VEC3}});

	checkGlError("Failed to init hole model");
}

void water::init(maze *m) {
    std::vector<glm::mat4> matrices;

    for (int i=0; i<m->datasize(); ++i) {
        tile *t = m->getTile(i);
        int x = i % m->width();
        int y = i / m->width();
        if (t->state == STATE_BLOCK) {
            matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3(x * tileSize, -blockHeight, y * tileSize)));
        }
    }

	vao.update_matrices(1, matrices.data(), matrices.size(), 1);
}

void water::render() {
	refractionSampler.bind(refraction);
	dudvSampler.bind(material::getTexture("TEX_WATER_DUDV"));
	normalSampler.bind(material::getTexture("TEX_WATER_NORMALS"));

	globalTime = SDL_GetTicks();
	m_shader.use();

	vao.draw_instances();
}