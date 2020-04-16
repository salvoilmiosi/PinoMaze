#include "world.h"

#include <glm/glm.hpp>
#include <iostream>

#include "../resources.h"
#include "../options.h"
#include "../game.h"

world::world(context *m_context, game *m_game) :
    m_context(m_context), m_game(m_game),

    m_shader(m_game),
    m_shadow("shadow", SHADER_RESOURCE(s_shadow_v), SHADER_RESOURCE(s_shadow_f)),

    m_water(m_context, m_game),
    m_skybox(m_game),
    m_particles(m_game)
{
    m_shadow.add_uniform("lightMatrix", &m_shader.m_light);

    initPillars();
    initGround();
    initWalls();
    initItems();

    m_bridge.init(m_game->m_maze);
    m_water.init(m_game->m_maze);

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
    m_wall.draw_instances();
    m_pillar.draw_instances();
    m_teleport.draw_instances();
    m_start.draw_instances();
    m_end.draw_instances();
    m_bridge.render_flat();

    if (m_game->teleportTimer % 18 < 9) {
        m_marble.draw_instances();
    }

    framebuffer::unbind();
    glViewport(0, 0, m_context->window_width, m_context->window_height);

    m_shader.shadowSampler.bind(m_shader.shadowMap);
}

void world::tick() {
    m_particles.tick();
}

void world::render() {
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
    
    m_shader.m_sun = m_game->sun;
    m_shader.m_sun.direction = glm::vec3(m_game->m_view * glm::vec4(m_game->sun.direction, 0.0));
    m_shader.m_light_biased = biasMatrix * m_shader.m_light;

    m_shader.apply_material("MAT_FLOOR");
    m_ground.draw_instances();

    m_shader.apply_material("MAT_PILLAR");
    m_pillar.draw_instances();

    m_shader.addFlags(DRAWING_WALLS);
    m_shader.apply_material("MAT_BRICKS");
    m_wall.draw_instances();
    m_shader.removeFlags(DRAWING_WALLS);

    m_shader.apply_material("MAT_START");
    m_start.draw_instances();

    m_shader.apply_material("MAT_END");
    m_end.draw_instances();

    m_shader.apply_material("MAT_ARROW");
    m_arrow.draw_instances();

    m_bridge.render(m_shader);

    m_shader.addFlags(DRAWING_TELEPORT);
    m_shader.tpTileSampler.bind(material::getTexture("TEX_TELEPORT_TILES"));
    m_shader.apply_material("MAT_TELEPORT");
    m_teleport.draw_instances();
    m_shader.removeFlags(DRAWING_TELEPORT);

    renderRefraction();
    framebuffer::unbind();
    glViewport(0, 0, m_context->window_width, m_context->window_height);
    m_water.render();

    m_shader.use();

    if (m_game->teleportTimer % 18 < 9) {
        m_shader.apply_material("MAT_MARBLE");
        m_marble.draw_instances();
    }

    m_particles.render();
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
    m_arrow.draw_instances();

    m_shader.apply_material("MAT_MARBLE");
    m_marble.draw_instances();

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

    m_pillar.update_matrices(2, matrices.data(), matrices.size(), 4);
}

void world::initGround() {
    std::vector<glm::mat4> matrices;

    for (int i=0; i<m_game->m_maze->datasize(); ++i) {
        tile *m_tile = m_game->m_maze->getTile(i);
        mazeItem *item = m_game->m_maze->findItem(m_tile);
        if (m_tile->state != STATE_BLOCK && !(item && item->type == ITEM_ARROW)) {
            int x = i % m_game->m_maze->width();
            int y = i / m_game->m_maze->width();
            matrices.push_back(glm::translate(glm::mat4(1.f), glm::vec3((x + .5f) * tileSize, -blockHeight, (y + .5f) * tileSize)));
        }
    }

    m_ground.update_matrices(2, matrices.data(), matrices.size(), 4);
}

void world::initWalls() {
    int x = 0;
    int y = 0;

    maze *m = m_game->m_maze;

    struct wall_instance {
        glm::mat4 matrix;
        float wallValue;
    };
    std::vector<wall_instance> instances;

    for (wall &w : m->hwalls) {
        for (x = 0; x<w.size(); ++x) {
            if (w[x]) {
                wall_instance data;
                data.matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, wallHeight / 2.f, y * tileSize));
                data.wallValue = w[x];
                instances.push_back(data);
            }
        }
        ++y;
    }

    x = 0;
    for (wall &w : m->vwalls) {
        for (y = 0; y < w.size(); ++y) {
            if (w[y]) {
                wall_instance data;
                data.matrix = glm::translate(glm::mat4(1.f), glm::vec3(x * tileSize, wallHeight / 2.f, (y + 0.5f) * tileSize));
                data.matrix = glm::rotate(data.matrix, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
                data.wallValue = w[y];
                instances.push_back(data);
            }
        }
        ++x;
    }

    m_wall.update_instances(2, instances.data(), instances.size() * sizeof(wall_instance), {{4, ATTR_MAT4}, {9, ATTR_FLOAT}});
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
        m_start.update_matrices(2, &matrix, 1, 4);
    }

    if ((t = m->endTile()) != nullptr) {
        index = m->getIndex(t);
        x = index % m->width();
        y = index / m->width();
        matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
        m_end.update_matrices(2, &matrix, 1, 4);
    }

    std::vector<glm::mat4> arrowMatrices;
    
    struct tp_instance {
        glm::mat4 matrix;
        glm::vec2 uv;
    };
    
    std::vector<tp_instance> tp_instances;

    for (std::pair<const int, mazeItem> &it : m->items) {
        switch(it.second.type) {
        case ITEM_ARROW:
            x = it.second.item.x;
            y = it.second.item.y;
            matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + .5f) * tileSize, -blockHeight, (y + .5f) * tileSize));
            matrix = matrix * glm::rotate(glm::mat4(1.f), glm::radians(90.f) * (it.second.arrow.direction + 1), glm::vec3(0.f, 1.f, 0.f));
            arrowMatrices.push_back(matrix);
            break;
        case ITEM_TELEPORT:
        {
            tp_instance tpdata;
            unsigned char c = it.second.teleport.tpChar;
            tpdata.uv.x = (1.f / 16.f) * (c % 16);
            tpdata.uv.y = (1.f / 16.f) * (c / 16);

            int x = it.second.item.x;
            int y = it.second.item.y;

            tpdata.matrix = glm::translate(glm::mat4(1.f), glm::vec3((x + 0.5f) * tileSize, 0.f, (y + 0.5f) * tileSize));

            tp_instances.push_back(tpdata);
            break;
        }
        default:
            break;
        }
    }

    m_arrow.update_matrices(2, arrowMatrices.data(), arrowMatrices.size(), 4);
    m_teleport.update_instances(2, tp_instances.data(), tp_instances.size() * sizeof(tp_instance), {{4, ATTR_MAT4}, {8, ATTR_VEC2}});
}
