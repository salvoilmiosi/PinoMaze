#include "renderer_world.h"

#include <glm/gtc/matrix_transform.hpp>

#include "res_loader.h"
#include "game.h"
#include "globals.h"

worldRenderer::worldRenderer(maze *m) : m(m), bridge(m) {
	pillarBox.setSize(pillarSize, pillarHeight, pillarSize, tileSize / 2.5f);
    groundBox.setSize(tileSize, blockHeight * 2.f, tileSize, tileSize);
    wallBox.setSize(tileSize - wallThickness, wallHeight, wallThickness, tileSize / 1.5f);
    startBox.setSize(startBoxSize, startBoxHeight, startBoxSize, startBoxSize);
    endBox.setSize(startBoxSize, startBoxHeight, startBoxSize, startBoxSize);
    arrowBox.setSize(startBoxSize, startBoxHeight, startBoxSize, startBoxSize);
    marble.setSize(marbleRadius, 16, 16);

	width = m->width();
	height = m->height();

	initPillars();
	initGround();
	initWalls();
	initItems();
}

bool worldRenderer::init() {
    if (!shader.init()) return false;
    if (!shadows.init()) return false;

    if (!shadowBuffer.init()) return false;
    shadowMap.setFilter(GL_NEAREST);
    shadowMap.setWrapParam(GL_CLAMP_TO_EDGE);
    shadowMap.createEmpty(1024, 1024, true);
    shadowBuffer.attachDepthMap(shadowMap);

	if (shader.bindProgram()) {
		shader.setShadowBias(0.003f);
		shader.setShadowTexelSize(glm::vec2(1.f / shadowMap.width()));
		shader.setTextureSampler(0);
		shader.setNormalSampler(1);
		shader.setShadowSampler(2);
		shader.unbindProgram();
	} else {
		return false;
	}

	if (!pillarBox.init()) return false;
	if (!groundBox.init()) return false;
	if (!wallBox.init()) return false;
	if (!bridge.init()) return false;
	if (!marble.init()) return false;
	if (!startBox.init()) return false;
	if (!endBox.init()) return false;
	if (!arrowBox.init()) return false;

	return checkGlError("Failed to init world renderer");
}

void worldRenderer::renderShadowmap(game *g) {
	float marbleX = g->marbleMatrix()[3][0];
	float marbleZ = g->marbleMatrix()[3][2];

    static const glm::mat4 lightProjection = glm::ortho(-shadowArea, shadowArea, -shadowArea, shadowArea, -shadowArea, shadowArea);
	static const float texelSize = shadowArea * 2.f / shadowMap.width();

    glm::vec3 center(marbleX, 0, marbleZ);
	center = glm::floor(center / texelSize) * texelSize;

    glm::mat4 lightView = glm::lookAt(g->worldLight().direction + center, center, glm::vec3(0.f, 1.f, 0.f));
    g->lightMatrix() = lightProjection * lightView;

	shadowBuffer.bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT);

	if (shadows.bindProgram()) {
		shadows.setLightMatrix(g->lightMatrix());

		wallBox.render();
		pillarBox.render();
		bridge.render();

		if (teleportTimer % 18 < 9) {
			marble.render();
		}

		shadows.unbindProgram();
	}

    shadowBuffer.unbindFramebuffer();
}

void worldRenderer::tick(game *g) {
	if (teleportTimer > 0) {
		--teleportTimer;
	}
	if (g->hasTeleported()) {
		teleportTimer = 72;
	}
}

void worldRenderer::render(game *g) {
	marble.loadMatrix(g->marbleMatrix());

    renderShadowmap(g);
	
	if (shader.bindProgram()) {
		shader.setProjectionMatrix(g->projectionMatrix());
		shader.setViewMatrix(g->viewMatrix());
		shader.setLightMatrix(biasMatrix * g->lightMatrix());

		shader.setLight(g->viewLight());

		shadowMap.bindTexture(2);

		shader.setMaterial(g->MAT_FLOOR);
		groundBox.render();

		shader.setMaterial(g->MAT_COBBLE);
		pillarBox.render();

		shader.setMaterial(g->MAT_BRICKS);
		wallBox.render();

		bridge.renderShader(g, shader);

		shader.setMaterial(g->MAT_START);
		startBox.render();

		shader.setMaterial(g->MAT_END);
		endBox.render();

		shader.setMaterial(g->MAT_ARROW);
		arrowBox.render();

		if (teleportTimer % 18 < 9) {
			shader.setMaterial(g->MAT_MARBLE);
			marble.render();
		}

		shader.unbindProgram();
	}
}

void worldRenderer::renderRefraction(game *g) {
	if (shader.bindProgram()) {
		glEnable(GL_CLIP_DISTANCE0);
		glDisable(GL_CULL_FACE);

		shader.setRefractionHeight(-blockHeight);

		shader.setMaterial(g->MAT_FLOOR_REFRACTED);
		groundBox.render();

		shader.setMaterial(g->MAT_MARBLE_REFRACTED);
		marble.render();

		shader.setRefractionHeight(999.f);

		glEnable(GL_CULL_FACE);
		glDisable(GL_CLIP_DISTANCE0);

		shader.unbindProgram();
	}
}

void worldRenderer::initPillars() {
    glm::mat4 identity;

    tile *t;
    mazeItem *item;

	for (int x = 0; x <= width; ++x) {
        for (int y = 0; y <= height; ++y) {
            bool hasWalls = false;
            if (x > 0 && m->hwalls[y][x-1]) hasWalls = true;
            else if (x < width && m->hwalls[y][x]) hasWalls = true;
            else if (y > 0 && m->vwalls[x][y-1]) hasWalls = true;
            else if (y < height && m->vwalls[x][y]) hasWalls = true;
            else {
                if ((t = m->getTile(x-1, y-1)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
                else if ((t = m->getTile(x, y-1)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
                else if ((t = m->getTile(x-1, y)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
                else if ((t = m->getTile(x, y)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
            }

            if (hasWalls) {
                pillarBox.addMatrix(glm::translate(identity,
                    glm::vec3(x * tileSize, pillarHeight / 2.f, y * tileSize)));
            }
        }
	}
}

void worldRenderer::initGround() {
    glm::mat4 identity;

    for (int i=0; i<m->datasize(); ++i) {
        if (m->getTile(i)->state != STATE_BLOCK) {
            int x = i % m->width();
            int y = i / m->width();
            groundBox.addMatrix(glm::translate(
                identity, glm::vec3((x + .5f) * tileSize, -blockHeight, (y + .5f) * tileSize)));
        }
    }
}

void worldRenderer::initWalls() {
    glm::mat4 matrix, identity;
    int x = 0;
    int y = 0;

    for (wall &w : m->hwalls) {
        for (x = 0; x<w.length(); ++x) {
            if (w[x]) {
                matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, wallHeight / 2.f, y * tileSize));
                wallBox.addMatrix(matrix);
            }
        }
        ++y;
    }

    x = 0;
    for (wall &w : m->vwalls) {
        for (y = 0; y < w.length(); ++y) {
            if (w[y]) {
                matrix = glm::translate(identity, glm::vec3(x * tileSize, wallHeight / 2.f, (y + 0.5f) * tileSize));
                matrix = glm::rotate(matrix, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
                wallBox.addMatrix(matrix);
            }
        }
        ++x;
    }
}

void worldRenderer::initItems() {
    glm::mat4 matrix, identity;

    tile *t;
    int index, x, y;
    if ((t = m->startTile()) != nullptr) {
        index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        startBox.addMatrix(matrix);
    }

    if ((t = m->endTile()) != nullptr) {
        index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        endBox.addMatrix(matrix);
    }

    for (pair<const int, mazeItem> &it : m->items) {
        switch(it.second.type) {
        case ITEM_ARROW:
            x = it.second.item.x;
            y = it.second.item.y;
            matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
            matrix = glm::rotate(matrix, glm::radians(90.f) * (it.second.arrow.direction + 1), glm::vec3(0.f, 1.f, 0.f));
            arrowBox.addMatrix(matrix);
            break;
        }
    }
}
