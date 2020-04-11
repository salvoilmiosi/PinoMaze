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

#include "world_shader.h"
#include "bridge.h"
#include "hole.h"
#include "skybox.h"

class world : public entity {
public:
    world(context *m_context, game *m_game);

public:
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

    world_shader m_shader;
    shader m_shadow;

    box box_pillar, box_ground, box_wall;
    box box_start, box_end, box_arrow, box_teleport;
    bridge m_bridge;
    hole m_hole;
    skybox m_skybox;
	sphere marble;
};

#endif // __SHADER_WORLD_H__
