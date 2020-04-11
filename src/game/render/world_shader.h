#ifndef __WORLD_SHADER_H__
#define __WORLD_SHADER_H__

#include "../engine/shader.h"
#include "../engine/material.h"
#include "../engine/framebuffer.h"

class world_shader : public shader {
public:
    world_shader(class game *m_game);

public:
    void apply_material(const char *mat_name);

private:
    glm::mat4 m_light;
    glm::mat4 m_light_biased;

    sampler diffuseSampler;
    sampler normalSampler;
    sampler shadowSampler;
    sampler tpTileSampler;
    
    framebuffer shadowBuffer;
	texture shadowMap;

    bool enableTexture = true;
    bool enableNormalMap = true;
    bool enableTpTiles = false;
    bool enableShadow = true;
    bool enableSpecular = true;
    
    float shadowBias = 0.003f;
    glm::vec2 shadowTexelSize;
    float refractionHeight;
    float tpTileSize = 1.f / 16.f;

    light m_sun;
    material m_material;

    friend class world;
};

#endif // __WORLD_SHADER_H___