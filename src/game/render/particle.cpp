#include "particle.h"

#include "../resources.h"
#include "../options.h"
#include "../game.h"

#include "material.h"

#include <iostream>
#include <vector>

#define MAX_PARTICLES 3000
#define NUM_SOURCES 5

#define TYPE_NONE           0.f
#define TYPE_SOURCE         1.f
#define TYPE_STATIC         2.f
#define TYPE_PARTICLE1      3.f
#define TYPE_PARTICLE2      4.f
#define TYPE_PARTICLE3      5.f
#define TYPE_PARTICLE4      6.f

struct particle {
    float type;
    float age;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 data;
    // type = source   : data = (num_particles, force, size, random_seed)
    // type = particle : data = (color rgb, size)
};

static constexpr vertex_attrib_list particle_attr_list {
    {0, ATTR_FLOAT}, {1, ATTR_FLOAT}, {2, ATTR_VEC3}, {3, ATTR_VEC3}, {4, ATTR_VEC4}};

particle_system::particle_system(game *m_game) :
    m_game(m_game),
    m_shader("particle", SHADER_RESOURCE(s_particle_v), SHADER_RESOURCE(s_particle_g),
        "type1", "age1", "position1", "velocity1", "data1"),
    m_billboard("billboard", SHADER_RESOURCE(s_billboard_v), SHADER_RESOURCE(s_billboard_g), SHADER_RESOURCE(s_billboard_f)),
    randomTexture(1024, 1)
{
    m_shader.add_uniform("randomTexture", &randomSampler);
    m_shader.add_uniform("deltaMs", &deltaMs);
    m_shader.add_uniform("globalTime", &globalTime);

    m_billboard.add_uniform("viewMatrix", &m_game->m_view);
    m_billboard.add_uniform("projMatrix", &m_game->m_proj);
    m_billboard.add_uniform("cameraPos", &m_game->m_camera.position);
    m_billboard.add_uniform("particleTexture", &particleSampler);

    for (size_t i=0; i<2; ++i) {
        tfbs[i].init_buffer(sizeof(particle) * MAX_PARTICLES, particle_attr_list);
    }

    checkGlError("Could not create transform feedback");

    createRandomTexture();
}

void particle_system::createRandomTexture() {
    std::vector<glm::vec3> data(randomTexture.width());

    for (size_t i=0; i<randomTexture.width(); ++i) {
        data[i].x = rand() / (float) RAND_MAX;
        data[i].y = rand() / (float) RAND_MAX;
        data[i].z = rand() / (float) RAND_MAX;
    }

    randomTexture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, randomTexture.width(), randomTexture.height(), 0, GL_RGB, GL_FLOAT, data.data());

    checkGlError("Could not create random Texture");
}

void particle_system::init() {
    std::vector<particle> sources;
    
    for (std::pair<const int, mazeItem> &it : m_game->m_maze->items) {
        if (it.second.type == ITEM_TELEPORT && it.second.teleport.warpX >= 0 && it.second.teleport.warpY >= 0) {
            particle source;

            source.type = TYPE_STATIC;
            source.age = 0.f;
            source.position = glm::vec3((it.second.item.x + 0.5f) * tileSize, teleportHeight, (it.second.item.y + 0.5f) * tileSize);
            source.velocity = glm::vec3(0.f);
            source.data.x = 20.f; // num_particles
            source.data.y = 0.6f; // force
            source.data.z = teleportRadius2 * 0.6f; // size
            source.data.w = (float) rand() / RAND_MAX; // random_seed

            sources.push_back(source);
        }
    }

    teleport_sources.update_vertices(0, sources.data(), sources.size(), particle_attr_list, false);
}

void particle_system::tick() {
	glm::mat4 matrix = m_game->m_marble;
	glm::vec3 position(matrix[3][0], matrix[3][1], matrix[3][2]);
	static glm::vec3 lastPosition = position;
    static glm::vec3 velocity(0.f);
    velocity *= 0.96f;
    velocity += position - lastPosition;
    lastPosition = position;
    
    particle sources[NUM_SOURCES];
    size_t i = 0;

    if (m_game->teleportTimer >= teleportTicks - 10) {
        velocity = glm::vec3(0.f);
        for (float count = 100.f; count > 0.f && i < NUM_SOURCES; count -= 50.f, ++i) {
            sources[i].type = TYPE_SOURCE;
            sources[i].age = 0.f;
            sources[i].position = position;
            sources[i].velocity = glm::vec3(0.f, 0.6f, 0.f);
            sources[i].data.x = MIN(count, 50.f); // num_particles
            sources[i].data.y = 0.8f; // force
            sources[i].data.z = marbleRadius; // size
            sources[i].data.w = rand() / (float) RAND_MAX; // random_seed
        }
	}
	if (m_game->won) {
        for (float count = rand() % 15 + 15; count > 0.f && i < NUM_SOURCES; count -= 50.f, ++i) {
            sources[i].type = TYPE_SOURCE;
            sources[i].age = 0.f;
            sources[i].position = position;
            sources[i].velocity = velocity + glm::vec3(0.f, 0.6f, 0.f);
            sources[i].data.x = MIN(count, 50.f); // num_particles
            sources[i].data.y = 0.8f; // force
            sources[i].data.z = marbleRadius; // size
            sources[i].data.w = rand() / (float) RAND_MAX; // random_seed
        }
    }

    marble_sources.update_vertices(0, sources, i, particle_attr_list, true);
}

void particle_system::render(float deltaNano) {
    static size_t currentVBO = 0;

    size_t currentTFB = (currentVBO + 1) & 1;

    deltaMs = deltaNano / 1000000.f;
    globalTime += deltaMs;

    randomSampler.bind(&randomTexture);

    m_shader.use();

    tfbs[currentTFB].start();

    teleport_sources.draw();
    teleport_sources.update_vertices(0, nullptr, 0, particle_attr_list, false);
    
    marble_sources.draw();
    marble_sources.update_vertices(0, nullptr, 0, particle_attr_list, true);
    
    tfbs[currentVBO].draw_feedback();
    
    tfbs[currentTFB].stop();

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    glDepthMask(GL_FALSE);

    particleSampler.bind(getTexture("TEX_PARTICLE_TEXTURE").get());
    m_billboard.use();

    tfbs[currentTFB].draw_feedback();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    currentVBO = currentTFB;
}