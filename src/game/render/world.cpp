#include "world.h"

#include <glm/glm.hpp>
#include <iostream>

#include "../resources.h"
#include "../options.h"
#include "../game.h"

world::world(engine_options *options, game *m_game) :
    options(options), m_game(m_game),

    m_shader(m_game),
    m_shadow("shadow", SHADER_RESOURCE(s_shadow_v), SHADER_RESOURCE(s_shadow_f),
        "lightMatrix", &m_shader.m_light),

    m_water(options, m_game),
    m_bridge(m_game),
    m_skybox(m_game),
    m_particles(m_game)
{
    m_particles.init();

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
    for (size_t i=0; i<numWallMaterials; ++i) {
        m_wall[i].draw_instances();
    }
    m_pillar.draw_instances();
    m_teleport.draw_instances();
    m_start.draw_instances();
    m_end.draw_instances();
    m_bridge.render_flat();

    if (m_game->teleportTimer % 18 < 9) {
        m_marble.draw_instances();
    }

    framebuffer::unbind();
    glViewport(0, 0, options->window_width, options->window_height);

    m_shader.shadowSampler.bind(&m_shader.shadowMap);
}

void world::tick() {
    int marble_x = m_game->m_marble[3][0] / tileSize;
    int marble_z = m_game->m_marble[3][2] / tileSize;

    static int old_gridx = -1;
    static int old_gridy = -1;

    int gridsize = 10;
    int gridx = MAX(0, (int)(marble_x / gridsize - 1) * gridsize);
    int gridy = MAX(0, (int)(marble_z / gridsize - 1) * gridsize);

    if (gridx != old_gridx || gridy != old_gridy) {
        load_models(gridx, gridy, gridsize * 3);
    }

    old_gridx = gridx;
    old_gridy = gridy;

    m_particles.tick();
}

void world::render(float deltaNano) {
    glClear(GL_DEPTH_BUFFER_BIT);
    m_skybox.render();

    static const glm::mat4 biasMatrix(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f);
        
    m_marble.update_matrices(2, &m_game->m_marble, 1, 4, true);

    renderShadowmap();
    m_shader.use();

    if (m_game->teleportTimer % 18 < 9) {
        m_shader.apply_material("MAT_MARBLE");
        m_marble.draw_instances();
    }
    
    m_shader.m_sun = m_game->sun;
    m_shader.m_sun.direction = glm::vec3(m_game->m_view * glm::vec4(m_game->sun.direction, 0.0));
    m_shader.m_light_biased = biasMatrix * m_shader.m_light;

    m_shader.apply_material("MAT_FLOOR");
    m_ground.draw_instances();

    m_shader.apply_material("MAT_PILLAR");
    m_pillar.draw_instances();

    m_shader.apply_material("MAT_WALL1");
    m_wall[0].draw_instances();

    m_shader.apply_material("MAT_WALL2");
    m_wall[1].draw_instances();

    m_shader.apply_material("MAT_WALL3");
    m_wall[2].draw_instances();

    m_shader.apply_material("MAT_START");
    m_start.draw_instances();

    m_shader.apply_material("MAT_END");
    m_end.draw_instances();

    m_shader.apply_material("MAT_ARROW");
    m_arrow.draw_instances();

    m_bridge.render(m_shader);

    m_shader.addFlags(ENABLE_TELEPORT);
    m_shader.tpTileSampler.bind(getTexture("TEX_TELEPORT_TILES").get());
    m_shader.apply_material("MAT_TELEPORT");
    m_teleport.draw_instances();
    m_shader.removeFlags(ENABLE_TELEPORT);

    renderRefraction();
    framebuffer::unbind();
    glViewport(0, 0, options->window_width, options->window_height);
    m_water.render(deltaNano);

    m_particles.render(deltaNano);
}

void world::renderRefraction() {
    m_water.bindRefraction();

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_skybox.render();
    m_shader.use();

    glEnable(GL_CLIP_DISTANCE0);
    glDisable(GL_CULL_FACE);

    m_shader.refractionHeight = -blockHeight;
    m_shader.apply_material("MAT_FLOOR");
    m_ground.draw_instances();

    m_shader.apply_material("MAT_ARROW");
    m_arrow.draw_instances();

    m_shader.apply_material("MAT_MARBLE");
    m_marble.draw_instances();

    m_shader.refractionHeight = 999.f;
    glEnable(GL_CULL_FACE);
    glDisable(GL_CLIP_DISTANCE0);
}

void world::load_models(int gridx, int gridy, int gridsize) {
    maze *m = m_game->m_maze;
    tile *t;
    mazeItem *item;

    std::vector<glm::mat4> matrices;

    int x = 0;
    int y = 0;

    for (x = gridx; x <= MIN(gridx + gridsize, m->width()); ++x) {
        for (y = gridy; y <= MIN(gridy + gridsize, m->height()); ++y) {
            bool hasWalls = false;
            if (x > 0 && m->hwalls[y][x-1].value) hasWalls = true;
            else if (x < m->width() && m->hwalls[y][x].value) hasWalls = true;
            else if (y > 0 && m->vwalls[x][y-1].value) hasWalls = true;
            else if (y < m->height() && m->vwalls[x][y].value) hasWalls = true;
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

    m_pillar.update_matrices(2, matrices.data(), matrices.size(), 4, true);

    checkGlError("Failed to load pillars");

    matrices.clear();

    for (y = gridy; y < MIN(gridy + gridsize, m->height()); ++y) {
        for (x = gridx; x < MIN(gridx + gridsize, m->width()); ++x) {
            tile *m_tile = m_game->m_maze->getTile(x, y);
            mazeItem *item = m_game->m_maze->findItem(m_tile);
            if (m_tile->state != STATE_BLOCK && !(item && item->type == ITEM_ARROW)) {
                matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3((x + .5f) * tileSize, -blockHeight, (y + .5f) * tileSize)));
            }
        }
    }

    m_ground.update_matrices(2, matrices.data(), matrices.size(), 4, true);

    checkGlError("Failed to load ground");

    std::vector<glm::mat4> wallMatrices[numWallMaterials];

    for (y = gridy; y < MIN(gridy + gridsize + 1, m->hwalls.size()); ++y) {
        wall &w = m->hwalls[y];
        for (x = gridx; x < MIN(gridx + gridsize, w.size()); ++x) {
            if (w[x].value) {
                glm::mat4 matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, wallHeight / 2.f, y * tileSize));
                wallMatrices[w[x].value - 1].push_back(matrix);
            }
        }
    }

    for (x = gridx; x < MIN(gridx + gridsize + 1, m->vwalls.size()); ++x) {
        wall &w = m->vwalls[x];
        for (y = gridy; y < MIN(gridy + gridsize, w.size()); ++y) {
            if (w[y].value > 0) {
                glm::mat4 matrix = glm::translate(glm::mat4(1.f), glm::vec3(x * tileSize, wallHeight / 2.f, (y + 0.5f) * tileSize));
                matrix = glm::rotate(matrix, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
                wallMatrices[w[y].value - 1].push_back(matrix);
            }
        }
    }

    for (size_t i=0; i<numWallMaterials; ++i) {
        m_wall[i].update_matrices(2, wallMatrices[i].data(), wallMatrices[i].size(), 4, true);
    }

    checkGlError("Failed to load walls");

    if ((t = m->startTile()) != nullptr) {
        size_t index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        glm::mat4 matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        m_start.update_matrices(2, &matrix, 1, 4, true);
    }

    if ((t = m->endTile()) != nullptr) {
        size_t index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        glm::mat4 matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        m_end.update_matrices(2, &matrix, 1, 4, true);
    }

    checkGlError("Failed to load start and end tiles");

    struct tp_instance {
        glm::mat4 matrix;
        glm::vec2 uv;
    };
    std::vector<glm::mat4> arrowMatrices;
    std::vector<tp_instance> tp_instances;

    for (std::pair<const int, mazeItem> &it : m->items) {
        x = it.second.item.x;
        y = it.second.item.y;
        if (x < gridx || x > gridx + gridsize || y < gridy || y > gridy + gridsize) continue;

        switch(it.second.type) {
        case ITEM_ARROW:
        {
            glm::mat4 matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + .5f) * tileSize, -blockHeight, (y + .5f) * tileSize));
            matrix = matrix * glm::rotate(glm::mat4(1.f), glm::radians(90.f) * (it.second.arrow.direction + 1), glm::vec3(0.f, 1.f, 0.f));
            arrowMatrices.push_back(matrix);
            break;
        }
        case ITEM_TELEPORT:
        {
            tp_instance tpdata;
            unsigned char c = it.second.teleport.tpChar;
            tpdata.uv.x = (1.f / 16.f) * (c % 16);
            tpdata.uv.y = (1.f / 16.f) * (c / 16);
            tpdata.matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, 0.f, (y + 0.5f) * tileSize));

            tp_instances.push_back(tpdata);
            break;
        }
        default:
            break;
        }
    }

    m_arrow.update_matrices(2, arrowMatrices.data(), arrowMatrices.size(), 4, true);
    m_teleport.update_instances(2, tp_instances.data(), tp_instances.size(), {{4, ATTR_MAT4}, {8, ATTR_VEC2}}, true);

    checkGlError("Failed to load items");

    m_bridge.load_models(gridx, gridy, gridsize);
    m_water.load_models(gridx, gridy, gridsize);
}
