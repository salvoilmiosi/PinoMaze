#ifndef __WORLD_H__
#define __WORLD_H__

#include <memory>

#include "../engine/entity.h"
#include "../engine/shader.h"
#include "../engine/material.h"
#include "../engine/box.h"
#include "../engine/sphere.h"
#include "../engine/framebuffer.h"

#include "../game.h"

#include "bridge.h"

class world : public entity {
public:
    world(context *m_context, game *m_game);

public:
    bool init();

	void tick();

    void render();
	void renderRefraction();

private:
    void renderShadowmap();

    void initPillars();
    void initGround();
    void initWalls();
    void initItems();

private:
    game *m_game;

    shader m_shader;
    shader m_shadow;

    box pillarBox;
    box groundBox;
    box wallBox;
    box startBox;
    box endBox;
    box arrowBox;
	sphere marble;
    bridge m_bridge;

    framebuffer shadowBuffer;

    glm::mat4 m_light;
    glm::mat4 m_light_biased;

    sampler diffuseSampler;
    sampler normalSampler;
    sampler shadowSampler;
	texture shadowMap;

    bool enableTexture;
    bool enableNormalMap;
    float shadowBias = 0.003f;
    glm::vec2 shadowTexelSize;
    float refractionHeight;

    light m_sun;
    material m_material;

	int teleportTimer = 0;
};

#endif // __SHADER_WORLD_H__
