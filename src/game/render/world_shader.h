#ifndef __WORLD_SHADER_H__
#define __WORLD_SHADER_H__

#include "../engine/shader.h"
#include "../engine/framebuffer.h"

#include "material.h"

#define ENABLE_TEXTURE      (1 << 0)
#define ENABLE_NORMALS      (1 << 1)
#define ENABLE_SHADOWS      (1 << 2)
#define ENABLE_SPECULAR     (1 << 3)
#define DRAWING_TELEPORT    (1 << 4)
#define DRAWING_WALLS       (1 << 5)

class world_shader : public vf_shader {
public:
    world_shader(class game *m_game);

public:
    void apply_material(const char *mat_name);

public:
    void addFlags(int flags) {
        renderFlags |= flags;
    }

    void removeFlags(int flags) {
        renderFlags &= ~flags;
    }

private:
    glm::mat4 m_light;
    glm::mat4 m_light_biased;

    sampler diffuseSampler{0};
    sampler normalSampler{1};
    sampler shadowSampler{2};
    sampler tpTileSampler{3};
    
    framebuffer shadowBuffer;
	texture shadowMap;

    int renderFlags = 0;
    
    float shadowBias = 0.003f;
    glm::vec2 shadowTexelSize;
    float refractionHeight;

    light m_sun;
    material m_material;

    friend class world;
};

#endif // __WORLD_SHADER_H___