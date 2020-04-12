#include "world.h"

#include <glm/glm.hpp>
#include <iostream>

#include "../resources.h"
#include "../options.h"
#include "../game.h"

world::world(context *m_context, game *m_game) :
    entity(m_context), m_game(m_game),

    m_shader(m_game),
    m_shadow("shadow", SHADER_RESOURCE(s_shadow_v), SHADER_RESOURCE(s_shadow_f)),

    m_skybox(m_game),
    m_hole(m_context, m_game),
    m_particles(m_game),

	box_pillar(pillarSize, pillarHeight, pillarSize, pillarHeight),
    box_ground(tileSize, blockHeight * 2.f, tileSize, tileSize),
    box_wall(tileSize - wallThickness, wallHeight, wallThickness, tileSize * 0.5f),
    box_start(startBoxSize, startBoxHeight, startBoxSize, startBoxSize),
    box_end(startBoxSize, startBoxHeight, startBoxSize, startBoxSize),
    box_arrow(startBoxSize, startBoxHeight, startBoxSize, startBoxSize),
    box_teleport(startBoxSize, startBoxHeight, startBoxSize, startBoxSize),
    marble(marbleRadius, 16, 16)
{
    m_shadow.add_uniform("lightMatrix", &m_shader.m_light);

    initPillars();
    initGround();
    initWalls();
    initItems();

    m_bridge.init(m_game->m_maze);
    m_hole.init(m_game->m_maze);

	checkGlError("Failed to init world renderer");
}

void world::renderShadowmap() {
	float marbleX = m_game->m_marble[3][0];
	float marbleZ = m_game->m_marble[3][2];

    static const glm::mat4 lightProjection = glm::ortho(-shadowArea, shadowArea, -shadowArea, shadowArea, -shadowArea, shadowArea);
	static const float texelSize = shadowArea * 2.f / m_shader.shadowMap.width();

    glm::vec3 center(marbleX, 0, marbleZ);
	center = glm::floor(center / texelSize) * texelSize;

    glm::mat4 lightView = glm::lookAt(m_game->sun.direction + center, center, glm::vec3(0.f, 1.f, 0.f));
    m_shader.m_light = lightProjection * lightView;

	m_shader.shadowBuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT);

    m_shadow.use();
    box_wall.draw_instances();
    box_pillar.draw_instances();
    m_bridge.render_flat();

    if (m_game->teleportTimer % 18 < 9) {
        marble.draw_instances();
    }

    framebuffer::unbind();
    glViewport(0, 0, m_context->window_width, m_context->window_height);

    m_shader.shadowSampler.bind(m_shader.shadowMap);
}

void world::tick() {
    m_hole.tick();
}

void world::render() {
	glClear(GL_DEPTH_BUFFER_BIT);
    m_skybox.render();

    static const glm::mat4 biasMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);
        
	marble.update_matrices(2, &m_game->m_marble, 1, 4, true);

    renderShadowmap();
	m_shader.use();
    
    m_shader.m_sun = m_game->sun;
    m_shader.m_sun.direction = glm::vec3(m_game->m_view * glm::vec4(m_game->sun.direction, 0.0));
    m_shader.m_light_biased = biasMatrix * m_shader.m_light;

    m_shader.apply_material("MAT_FLOOR");
    box_ground.draw_instances();

    m_shader.apply_material("MAT_PILLAR");
    box_pillar.draw_instances();

    m_shader.apply_material("MAT_BRICKS");
    box_wall.draw_instances();

    m_shader.apply_material("MAT_START");
    box_start.draw_instances();

    m_shader.apply_material("MAT_END");
    box_end.draw_instances();

    m_shader.apply_material("MAT_ARROW");
    box_arrow.draw_instances();

    m_bridge.render(m_shader);

    m_shader.enableTpTiles = true;
    m_shader.tpTileSampler.bind(material::getTexture("TEX_TELEPORT_TILES"));
    m_shader.apply_material("MAT_RUST");
    box_teleport.draw_instances();
    m_shader.enableTpTiles = false;

    renderRefraction();
    framebuffer::unbind();
    glViewport(0, 0, m_context->window_width, m_context->window_height);
    m_hole.render();

    m_shader.use();

    // if (m_game->teleportTimer % 18 < 9) {
    //     m_shader.apply_material("MAT_MARBLE");
    //     marble.draw_instances();
    // }

    m_particles.render();
}

void world::renderRefraction() {
    m_hole.bindRefraction();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_skybox.render();
    m_shader.use();

    glEnable(GL_CLIP_DISTANCE0);
    glDisable(GL_CULL_FACE);

    m_shader.refractionHeight = -blockHeight;
    m_shader.apply_material("MAT_FLOOR");
    box_ground.draw_instances();

    m_shader.apply_material("MAT_MARBLE");
    marble.draw_instances();

    m_shader.refractionHeight = 999.f;
    glEnable(GL_CULL_FACE);
    glDisable(GL_CLIP_DISTANCE0);
}

void world::initPillars() {
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
                matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3(x * tileSize, pillarHeight / 2.f, y * tileSize)));
            }
        }
	}

    box_pillar.update_matrices(2, matrices.data(), matrices.size(), 4);
}

void world::initGround() {
    std::vector<glm::mat4> matrices;

    for (int i=0; i<m_game->m_maze->datasize(); ++i) {
        if (m_game->m_maze->getTile(i)->state != STATE_BLOCK) {
            int x = i % m_game->m_maze->width();
            int y = i / m_game->m_maze->width();
            matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3((x + .5f) * tileSize, -blockHeight, (y + .5f) * tileSize)));
        }
    }

    box_ground.update_matrices(2, matrices.data(), matrices.size(), 4);
}

void world::initWalls() {
    glm::mat4 matrix;
    int x = 0;
    int y = 0;

    maze *m = m_game->m_maze;

    std::vector<glm::mat4> matrices;

    for (wall &w : m->hwalls) {
        for (x = 0; x<w.length(); ++x) {
            if (w[x]) {
                matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, wallHeight / 2.f, y * tileSize)));
            }
        }
        ++y;
    }

    x = 0;
    for (wall &w : m->vwalls) {
        for (y = 0; y < w.length(); ++y) {
            if (w[y]) {
                matrix = glm::translate(glm::mat4(1.f), glm::vec3(x * tileSize, wallHeight / 2.f, (y + 0.5f) * tileSize));
                matrix = glm::rotate(matrix, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
                matrices.push_back(matrix);
            }
        }
        ++x;
    }

    box_wall.update_matrices(2, matrices.data(), matrices.size(), 4);
}

void world::initItems() {
    glm::mat4 matrix;

    maze *m = m_game->m_maze;

    tile *t;
    int index, x, y;
    if ((t = m->startTile()) != nullptr) {
        index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        box_start.update_matrices(2, &matrix, 1, 4);
    }

    if ((t = m->endTile()) != nullptr) {
        index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        box_end.update_matrices(2, &matrix, 1, 4);
    }

    std::vector<glm::mat4> arrowMatrices;
    
    struct tp_instance_data {
        glm::mat4 matrix;
        glm::vec2 uv;
    };
    
    std::vector<tp_instance_data> tp_instances;

    for (std::pair<const int, mazeItem> &it : m->items) {
        switch(it.second.type) {
        case ITEM_ARROW:
            x = it.second.item.x;
            y = it.second.item.y;
            matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
            matrix = matrix * glm::rotate(glm::mat4(1.f), glm::radians(90.f) * (it.second.arrow.direction + 1), glm::vec3(0.f, 1.f, 0.f));
            arrowMatrices.push_back(matrix);
            break;
        case ITEM_TELEPORT:
        {
            tp_instance_data tpdata;
			unsigned char c = it.second.teleport.tpChar;
			tpdata.uv.x = (1.f / 16.f) * (c % 16);
			tpdata.uv.y = (1.f / 16.f) * (c / 16);

			int x = it.second.item.x;
			int y = it.second.item.y;

			tpdata.matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));

            tp_instances.push_back(tpdata);
            break;
        }
        default:
            break;
        }
    }

    box_arrow.update_matrices(2, arrowMatrices.data(), arrowMatrices.size(), 4);
    box_teleport.update_instances(2, tp_instances.data(), tp_instances.size() * sizeof(tp_instance_data), {{4, ATTR_MAT4}, {8, ATTR_VEC2}});
}
