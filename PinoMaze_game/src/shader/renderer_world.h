#ifndef __RENDERER_WORLD_H__
#define __RENDERER_WORLD_H__

#include <glm/glm.hpp>

#include "shader_world.h"
#include "shader_shadow.h"
#include "framebuffer.h"

#include "box.h"
#include "sphere.h"
#include "renderer_bridge.h"
#include "maze.h"

class worldRenderer {
private:
    box pillarBox;
    box groundBox;
    box wallBox;
    box startBox;
    box endBox;
    box arrowBox;
	sphere marble;

	bridgeRenderer bridge;

    maze *m = nullptr;

    int width;
    int height;

	int teleportTimer = 0;

    worldShader shader;
    shadowShader shadows;
    framebuffer shadowBuffer;
	texture shadowMap;

public:
    worldRenderer(maze *m);

public:
    bool init();

	void tick(class game *g);

    void render(class game *g);
	void renderRefraction(class game *g);

private:
    void renderShadowmap(class game *g);

    void initPillars();
    void initGround();
    void initWalls();
    void initItems();
};

#endif // __RENDERER_WORLD_H__
