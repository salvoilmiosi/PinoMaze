#include "particle.h"

#include "../engine/context.h"

#include "../resources.h"
#include "../options.h"
#include "../game.h"

#include "material.h"

#include <iostream>

#define MAX_PARTICLES 2000
#define NUM_SOURCES 5

#define TYPE_NONE      0.f
#define TYPE_SOURCE    1.f
#define TYPE_PARTICLE1 2.f
#define TYPE_PARTICLE2 3.f
#define TYPE_PARTICLE3 4.f
#define TYPE_PARTICLE4 5.f

struct particle {
    float type;
    float age;              // for source : number of particles to generate
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;        // for source : (force, random, random)
    float size;
};

static constexpr vertex_attrib_list particle_attr_list {
    {0, ATTR_FLOAT}, {1, ATTR_FLOAT}, {2, ATTR_VEC3}, {3, ATTR_VEC3}, {4, ATTR_VEC3}, {5, ATTR_FLOAT}};

particle_system::particle_system(game *m_game) :
    m_game(m_game),
    m_particle("particle", SHADER_RESOURCE(s_particle_v), SHADER_RESOURCE(s_particle_g),
        "type1", "age1", "position1", "velocity1", "color1", "size1"),
    m_billboard("billboard", SHADER_RESOURCE(s_billboard_v), SHADER_RESOURCE(s_billboard_g), SHADER_RESOURCE(s_billboard_f)),
    randomTexture(1024, 1)
{
    m_particle.add_uniform("randomTexture", &randomSampler.gl_samplerid);
    m_particle.add_uniform("deltaMs", &deltaMs);
    m_particle.add_uniform("globalTime", &globalTime);

    m_billboard.add_uniform("viewMatrix", &m_game->m_view);
    m_billboard.add_uniform("projMatrix", &m_game->m_proj);
    m_billboard.add_uniform("cameraPos", &m_game->m_camera.position);
    m_billboard.add_uniform("particleTexture", &particleSampler.gl_samplerid);

    for (size_t i=0; i<2; ++i) {
        tfbs[i].init_buffer(sizeof(particle) * MAX_PARTICLES, particle_attr_list);
    }

    checkGlError("Could not create transform feedback");

    createRandomTexture();
}

void particle_system::createRandomTexture() {
    glm::vec3 *data = new glm::vec3[randomTexture.width()];

    for (size_t i=0; i<randomTexture.width(); ++i) {
        data[i].x = rand() / (float) RAND_MAX;
        data[i].y = rand() / (float) RAND_MAX;
        data[i].z = rand() / (float) RAND_MAX;
    }

    randomTexture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, randomTexture.width(), randomTexture.height(), 0, GL_RGB, GL_FLOAT, data);
    delete []data;

    checkGlError("Could not create random Texture");
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
            sources[i].age = MIN(count, 50.f);
            sources[i].position = position;
            sources[i].velocity = glm::vec3(0.f, 0.6f, 0.f);
            sources[i].color.x = 0.8f;
            sources[i].color.y = rand() / (float) RAND_MAX;
            sources[i].color.z = rand() / (float) RAND_MAX;
            sources[i].size = marbleRadius;
        }
	}
	if (m_game->won) {
        for (float count = rand() % 15 + 15; count > 0.f && i < NUM_SOURCES; count -= 50.f, ++i) {
            sources[i].type = TYPE_SOURCE;
            sources[i].age = MIN(count, 50.f);
            sources[i].position = position;
            sources[i].velocity = velocity + glm::vec3(0.f, 0.6f, 0.f);
            sources[i].color.x = 0.3f;
            sources[i].color.y = rand() / (float) RAND_MAX;
            sources[i].color.z = rand() / (float) RAND_MAX;
            sources[i].size = marbleRadius;
        }
    }

    source_vao.update_vertices(0, sources, sizeof(particle) * i, particle_attr_list, true);
}

void particle_system::render() {
    static size_t currentVBO = 0;

    size_t currentTFB = (currentVBO + 1) & 1;

    static float lastTime = SDL_GetTicks();
    globalTime = SDL_GetTicks();
    deltaMs = globalTime - lastTime;
    lastTime = globalTime;

    randomSampler.bind(randomTexture);

    m_particle.use();

    tfbs[currentTFB].start();

    source_vao.draw();
    source_vao.update_vertices(0, nullptr, 0, particle_attr_list, true);
    
    tfbs[currentVBO].draw_feedback();
    
    tfbs[currentTFB].stop();

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    glDepthMask(GL_FALSE);

    particleSampler.bind(material::getTexture("TEX_PARTICLE_TEXTURE"));
    m_billboard.use();

    tfbs[currentTFB].draw_feedback();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    currentVBO = currentTFB;
}