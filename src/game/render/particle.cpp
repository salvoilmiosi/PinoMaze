#include "particle.h"

#include "../engine/material.h"

#include "../resources.h"
#include "../game.h"

#include <iostream>

#define MAX_PARTICLES 1000

#define TYPE_NONE 0.f
#define TYPE_SOURCE 1.f
#define TYPE_PARTICLE1 2.f
#define TYPE_PARTICLE2 3.f
#define TYPE_PARTICLE3 4.f
#define TYPE_PARTICLE4 5.f

struct particle {
    float type;
    float age;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float size;
};

static constexpr std::initializer_list<vertex_attrib> particle_attr_list {
    {0, ATTR_FLOAT}, {1, ATTR_FLOAT}, {2, ATTR_VEC3}, {3, ATTR_VEC3}, {4, ATTR_VEC3}, {5, ATTR_FLOAT}};

particle_system::particle_system(game *m_game) :
    m_game(m_game),
    m_particle("particle", SHADER_RESOURCE(s_particle_v), SHADER_RESOURCE(s_particle_g), SHADER_RESOURCE(s_particle_f),
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
        tfbs[i].update_buffer(nullptr, sizeof(particle) * MAX_PARTICLES, particle_attr_list);
    }

    checkGlError("Could not create transform feedback");

    createRandomTexture();
}

void particle_system::createRandomTexture() {
    glm::vec3 *data = new glm::vec3[randomTexture.width()];

    for (size_t i=0; i<randomTexture.width(); ++i) {
        data[i].x = (rand() % 255) / 255.f;
        data[i].y = (rand() % 255) / 255.f;
        data[i].z = (rand() % 255) / 255.f;
    }

    randomTexture.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, randomTexture.width(), randomTexture.height(), 0, GL_RGB, GL_FLOAT, data);
    delete []data;

    checkGlError("Could not create random Texture");
}

void particle_system::tick() {
	static int enableTimer = 0;

	if (enableTimer > 0) --enableTimer;

	if (m_game->teleportTimer >= 72) {
		enableTimer = 20;
	}

	enabled = m_game->won || enableTimer > 0;
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

    glEnable(GL_RASTERIZER_DISCARD);
    tfbs[currentTFB].start();
    static bool first = true;
    
    glm::vec3 marblePosition = glm::vec3(m_game->m_marble[3][0], m_game->m_marble[3][1], m_game->m_marble[3][2]);
    static glm::vec3 lastPos = marblePosition;
    static glm::vec3 velocity;
    velocity = (velocity + marblePosition - lastPos) * 0.992f;
    lastPos = marblePosition;

    if (enabled) {

        particle source;
        source.type = TYPE_SOURCE;
        source.age = 5.f;
        source.position = marblePosition;
        source.velocity = velocity + glm::vec3(0.f, 0.3f, 0.f);

        tfbs[currentVBO].update_vertices(0, &source, sizeof(particle), particle_attr_list, true);
        tfbs[currentVBO].draw(0, 1);
    }
    
    if (!first) {
        tfbs[currentVBO].draw_feedback();
    }
        
    first = false;
    transform_feedback::stop();
    glDisable(GL_RASTERIZER_DISCARD);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    particleSampler.bind(material::getTexture("TEX_PARTICLE_TEXTURE"));
    m_billboard.use();

    tfbs[currentTFB].draw_feedback();
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    currentVBO = currentTFB;
}