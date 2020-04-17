#ifndef __WORLD_H__
#define __WORLD_H__

#include <memory>

#include "../engine/shader.h"
#include "../engine/box.h"
#include "../engine/sphere.h"
#include "../engine/trunc_cone.h"
#include "../engine/framebuffer.h"

#include "../game.h"
#include "../options.h"

#include "world_shader.h"
#include "material.h"
#include "bridge.h"
#include "water.h"
#include "skybox.h"
#include "particle.h"

class world {
public:
    world(context *m_context, game *m_game);

public:
    void tick();
    void render(float deltaMs);
	void renderRefraction();

private:
    void renderShadowmap();

    void initPillars();
    void initGround();
    void initWalls();
    void initItems();

private:
    context *m_context;
    game *m_game;

    world_shader m_shader;
    vf_shader m_shadow;

    water m_water;
    bridge m_bridge;
    skybox m_skybox;
    particle_system m_particles;

    box m_pillar {pillarSize, pillarHeight, pillarSize, pillarHeight};
    box m_ground {tileSize, blockHeight * 2.f, tileSize, tileSize};
    box m_wall {tileSize - wallThickness, wallHeight, wallThickness, tileSize * 0.5f};
    box m_start {startBoxSize, startBoxHeight, startBoxSize, startBoxSize};
    box m_end {startBoxSize, startBoxHeight, startBoxSize, startBoxSize};
    box m_arrow {tileSize, blockHeight * 2.f, tileSize, tileSize};
    trunc_cone m_teleport {teleportRadius1, teleportRadius2, teleportHeight, circleSubs};
	sphere m_marble {marbleRadius, circleSubs, circleSubs};
};

#endif // __SHADER_WORLD_H__
