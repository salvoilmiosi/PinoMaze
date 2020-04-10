#include "world.h"

#include <glm/glm.hpp>

#include "../resources.h"
#include "../options.h"
#include "../game.h"

world::world(context *m_context, game *m_game) :
    entity(m_context), m_game(m_game),

    m_shader("world", SHADER_RESOURCE(s_world_v), SHADER_RESOURCE(s_world_f)),
    m_shadow("shadow", SHADER_RESOURCE(s_shadow_v), SHADER_RESOURCE(s_shadow_f)),

	pillarBox(pillarSize, pillarHeight, pillarSize, pillarHeight),
    groundBox(tileSize, blockHeight * 2.f, tileSize, tileSize),
    wallBox(tileSize - wallThickness, wallHeight, wallThickness, tileSize * 0.5f),
    startBox(startBoxSize, startBoxHeight, startBoxSize, startBoxSize),
    endBox(startBoxSize, startBoxHeight, startBoxSize, startBoxSize),
    arrowBox(startBoxSize, startBoxHeight, startBoxSize, startBoxSize),
    marble(marbleRadius, 16, 16),

    diffuseSampler(0),
    normalSampler(1),
    shadowSampler(2)
{
    m_shader.add_uniform("projectionMatrix", &m_game->m_proj);
    m_shader.add_uniform("viewMatrix", &m_game->m_view);
    m_shader.add_uniform("lightMatrix", &m_light_biased);
    m_shader.add_uniform("diffuseTexture", &diffuseSampler.gl_samplerid);
    m_shader.add_uniform("normalTexture", &normalSampler.gl_samplerid);
    m_shader.add_uniform("shadowMap", &shadowSampler.gl_samplerid);
    m_shader.add_uniform("enableTexture", &enableTexture);
    m_shader.add_uniform("enableNormalMap", &enableNormalMap);

    m_shader.add_uniform("sun.ambient", (glm::vec3 *) &m_sun.ambient);
    m_shader.add_uniform("sun.diffuse", (glm::vec3 *) &m_sun.diffuse);
    m_shader.add_uniform("sun.specular", (glm::vec3 *) &m_sun.specular);
    m_shader.add_uniform("sun.direction", &m_sun.direction);

    m_shader.add_uniform("mat.ambient", (glm::vec3 *) &m_material.ambient);
    m_shader.add_uniform("mat.diffuse", (glm::vec3 *) &m_material.diffuse);
    m_shader.add_uniform("mat.specular", (glm::vec3 *) &m_material.specular);
    m_shader.add_uniform("mat.emissive", (glm::vec3 *) &m_material.emissive);
    m_shader.add_uniform("mat.shininess", &m_material.shininess);

    m_shader.add_uniform("shadowBias", &shadowBias);
    m_shader.add_uniform("shadowTexelSize", &shadowTexelSize);
    m_shader.add_uniform("refractionHeight", &refractionHeight);
    
    m_shadow.add_uniform("lightMatrix", &m_light_biased);

    shadowMap.setFilter(GL_NEAREST);
    shadowMap.setWrapParam(GL_CLAMP_TO_EDGE);
    shadowMap.createEmpty(4096, 4096, true);
    shadowBuffer.attachDepthMap(shadowMap);

    shadowBias = 0.003f;
    shadowTexelSize = glm::vec2(1.f / shadowMap.width());

    initPillars();
    initGround();
    initWalls();
    initItems();

	checkGlError("Failed to init world renderer");
}

void world::renderShadowmap() {
	float marbleX = m_game->m_marble[3][0];
	float marbleZ = m_game->m_marble[3][2];

    static const glm::mat4 lightProjection = glm::ortho(-shadowArea, shadowArea, -shadowArea, shadowArea, -shadowArea, shadowArea);
	static const float texelSize = shadowArea * 2.f / shadowMap.width();

    glm::vec3 center(marbleX, 0, marbleZ);
	center = glm::floor(center / texelSize) * texelSize;

    glm::mat4 lightView = glm::lookAt(m_game->sun.direction + center, center, glm::vec3(0.f, 1.f, 0.f));
    m_light = lightProjection * lightView;

	shadowBuffer.bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT);

    m_shadow.use_program();
    wallBox.draw();
    pillarBox.draw();
    m_bridge.drawFlat();

    if (teleportTimer % 18 < 9) {
        marble.draw();
    }

    shadowBuffer.unbindFramebuffer(m_context);
}

void world::tick() {
	if (teleportTimer > 0) {
		--teleportTimer;
	}
	if (m_game->hasTeleported()) {
		teleportTimer = 72;
	}
}

void world::render() {
	m_shader.use_program();

    static const glm::mat4 biasMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);

	marble.update_matrices(&m_game->m_marble, 1, 4);

    renderShadowmap();
    
    m_sun = m_game->sun;
    m_sun.direction = glm::vec3(m_game->m_view * glm::vec4(m_game->sun.direction, 0.0));
    m_light_biased = biasMatrix * m_light;
    
    shadowSampler.bindTexture(shadowMap);

    m_material = material::get("MAT_FLOOR");
    m_shader.update_uniforms();
    groundBox.draw();

    m_material = material::get("MAT_PILLAR");
    m_shader.update_uniforms();
    pillarBox.draw();

    m_material = material::get("MAT_BRICKS");
    m_shader.update_uniforms();
    wallBox.draw();

    m_bridge.draw(m_shader, m_material);

    m_material = material::get("MAT_START");
    m_shader.update_uniforms();
    startBox.draw();

    m_material = material::get("MAT_END");
    m_shader.update_uniforms();
    endBox.draw();

    m_material = material::get("MAT_ARROW");
    m_shader.update_uniforms();
    arrowBox.draw();

    if (teleportTimer % 18 < 9) {
        m_material = material::get("MAT_MARBLE");
        marble.draw();
    }
}

void world::renderRefraction() {
    m_shader.use_program();

    glEnable(GL_CLIP_DISTANCE0);
    glDisable(GL_CULL_FACE);

    refractionHeight = -blockHeight;
    m_material = material::get("MAT_FLOOR");
    m_shader.update_uniforms();
    groundBox.draw();

    m_material = material::get("MAT_MARBLE");
    m_shader.update_uniforms();
    marble.draw();

    refractionHeight = 999.f;
    glEnable(GL_CULL_FACE);
    glDisable(GL_CLIP_DISTANCE0);
}

void world::initPillars() {
    glm::mat4 identity;

    tile *t;
    mazeItem *item;

    maze *m = m_game->m_maze;

    std::vector<glm::mat4> matrices;

	for (int x = 0; x <= m->width(); ++x) {
        for (int y = 0; y <= m->height(); ++y) {
            bool hasWalls = false;
            if (x > 0 && m->hwalls[y][x-1]) hasWalls = true;
            else if (x < m->width() && m->hwalls[y][x]) hasWalls = true;
            else if (y > 0 && m->vwalls[x][y-1]) hasWalls = true;
            else if (y < m->height() && m->vwalls[x][y]) hasWalls = true;
            else {
                if ((t = m->getTile(x-1, y-1)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
                else if ((t = m->getTile(x, y-1)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
                else if ((t = m->getTile(x-1, y)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
                else if ((t = m->getTile(x, y)) && (item = m->findItem(t)) && item->type == ITEM_BRIDGE) hasWalls = true;
            }

            if (hasWalls) {
                matrices.push_back(glm::translate(identity,
                    glm::vec3(x * tileSize, pillarHeight / 2.f, y * tileSize)));
            }
        }
	}

    pillarBox.update_matrices(matrices.data(), matrices.size(), 4);
}

void world::initGround() {
    glm::mat4 identity;

    std::vector<glm::mat4> matrices;

    for (int i=0; i<m_game->m_maze->datasize(); ++i) {
        if (m_game->m_maze->getTile(i)->state != STATE_BLOCK) {
            int x = i % m_game->m_maze->width();
            int y = i / m_game->m_maze->width();
            matrices.push_back(glm::translate(
                identity, glm::vec3((x + .5f) * tileSize, -blockHeight, (y + .5f) * tileSize)));
        }
    }

    groundBox.update_matrices(matrices.data(), matrices.size(), 4);
}

void world::initWalls() {
    glm::mat4 matrix, identity;
    int x = 0;
    int y = 0;

    maze *m = m_game->m_maze;

    std::vector<glm::mat4> matrices;

    for (wall &w : m->hwalls) {
        for (x = 0; x<w.length(); ++x) {
            if (w[x]) {
                matrices.push_back(glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, wallHeight / 2.f, y * tileSize)));
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
                matrices.push_back(matrix);
            }
        }
        ++x;
    }

    wallBox.update_matrices(matrices.data(), matrices.size(), 4);
}

void world::initItems() {
    glm::mat4 matrix, identity;

    maze *m = m_game->m_maze;

    tile *t;
    int index, x, y;
    if ((t = m->startTile()) != nullptr) {
        index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        startBox.update_matrices(&matrix, 1, 4);
    }

    if ((t = m->endTile()) != nullptr) {
        index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        endBox.update_matrices(&matrix, 1, 4);
    }

    for (std::pair<const int, mazeItem> &it : m->items) {
        switch(it.second.type) {
        case ITEM_ARROW:
            x = it.second.item.x;
            y = it.second.item.y;
            matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
            matrix = glm::rotate(matrix, glm::radians(90.f) * (it.second.arrow.direction + 1), glm::vec3(0.f, 1.f, 0.f));
            arrowBox.update_matrices(&matrix, 1, 4);
            break;
        default:
            break;
        }
    }
}
