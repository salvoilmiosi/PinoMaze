#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "../engine/shader.h"
#include "../engine/texture.h"
#include "../engine/transform_feedback.h"

#include <glm/glm.hpp>

class particle_system {
public:
    particle_system(class game *m_game);

public:
    void tick();
    void render(float deltaNano);

private:
    void createRandomTexture();

private:
    class game *m_game;
    
    vertex_array source_vao{DRAW_POINTS};
    transform_feedback_shader m_particle;
    geom_shader m_billboard;

    texture randomTexture;

    sampler randomSampler{0};
    sampler particleSampler{0};

    transform_feedback tfbs[2];

    float deltaMs;
    float globalTime;
};

#endif // __PARTICLE_H__