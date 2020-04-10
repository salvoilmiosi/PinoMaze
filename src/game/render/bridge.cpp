#include "bridge.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <cstdio>

#include "../options.h"
#include "../engine/material.h"
#include "../engine/shader.h"
#include "../engine/context.h"

bridge::bridge() {
    float l = tileSize - wallThickness;
    float l2 = l + bridgeArcThickness;
    float h2 = l2 / l * bridgeArcHeight;

    std::vector<vertex> vertices;
    std::vector<GLuint> indices;

    overArc_offset = 0;
    drawArc(vertices, indices, -l/2.f, l/2.f, l2, h2, tileSize / 1.5f, true);

    underArc_offset = indices.size();
    drawArc(vertices, indices, -l/2.f, l/2.f, l, bridgeArcHeight, l, false);

    m_arc.calculate_buffers(vertices.data(), vertices.size(), indices.data(), indices.size());

    vertices.clear();
    indices.clear();

	drawArc(vertices, indices, wallThickness / 2.f, -wallThickness / 2.f, l, bridgeArcHeight, tileSize / 1.5f, false);
    drawTopWall(vertices, indices, wallThickness / 2.f, -wallThickness /2.f, l, l / 1.5f);

    drawArcWall(vertices, indices, -wallThickness / 2.f, -l, tileSize * 0.5f);
    drawArcWall(vertices, indices, wallThickness / 2.f, l, tileSize * 0.5f);

    m_wall.calculate_buffers(vertices.data(), vertices.size(), indices.data(), indices.size());

	checkGlError("Failed to init bridge model");
}

void bridge::init(maze *m) {
	glm::mat4 identity, matrix;
	bool wallUp, wallDown;

    std::vector<glm::mat4> matrices;
    std::vector<glm::mat4> wallMatrices;

	for (std::pair<const int, mazeItem> &it : m->items) {
		if (it.second.type == ITEM_BRIDGE) {
			int x = it.second.item.x;
			int y = it.second.item.y;

			matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, 0.f, (y + 0.5f) * tileSize));
			wallUp = !m->hwalls[y][x];
			wallDown = !m->hwalls[y + 1][x];

			tile *tileDown = m->getTile(x, y + 1);
			mazeItem *item;
			if (tileDown && (item = m->findItem(tileDown)) && item->type == ITEM_BRIDGE) {
				wallDown = false;
			}

			matrices.push_back(matrix);
			if (wallUp) wallMatrices.push_back(glm::translate(matrix, glm::vec3(0.f, 0.f, -tileSize * 0.5f)));
			if (wallDown) wallMatrices.push_back(glm::translate(matrix, glm::vec3(0.f, 0.f, tileSize * 0.5f)));
		}
	}

    m_arc.update_matrices(matrices.data(), matrices.size(), 4);
    m_wall.update_matrices(wallMatrices.data(), wallMatrices.size(), 4);
}

void bridge::drawFlat() {
    m_arc.draw();
    m_wall.draw();
}

void bridge::draw(class shader &m_shader, class material &m_material) {
    m_material = material::get("MAT_TILES");
    m_shader.update_uniforms();
    m_arc.draw(underArc_offset - overArc_offset, overArc_offset);
    m_material = material::get("MAT_CEILING");
    m_shader.update_uniforms();
    m_arc.draw(0, underArc_offset);
    m_material = material::get("MAT_BRICKS");
    m_shader.update_uniforms();
    m_wall.draw();
}

void bridge::drawArc(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float h, float texSize, bool ext) {
    GLuint startIndex = (GLuint)vertices.size();

    glm::vec3 position;

	float maxV = abs(z1 - z2) / texSize;

	float angle;

	for (int i=0; i <= bridgeSubsH; ++i) {
        angle = M_PI / bridgeSubsH * i;

		position = {cosf(angle) * w / 2.f, sinf(angle) * h, 0.f};

        float u = angle * bridgeArcHeight / texSize;

        vertex v1 = {{position.x, position.y, MAX(z1, z2)}, {u, 0}};
        vertex v2 = {{position.x, position.y, MIN(z1, z2)}, {u, maxV}};

        vertices.push_back(v1);
        vertices.push_back(v2);
	}

	addIndices(vertices, indices, startIndex, ext);
}

void bridge::drawArcWall(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z, float w, float texSize) {
    float a = bridgeWallHeight / bridgeArcHeight * abs(w / 2.f);
    float b = bridgeWallHeight;

    vertex v1, v2;
    for (int i=0; i<bridgeSubsV; ++i) {
        GLuint startIndex = (GLuint)vertices.size();
        for (int j=0; j<=bridgeSubsH; ++j) {
            float angle_bottom = M_PI * j / bridgeSubsH;

            float x = cosf(angle_bottom);
            float vx = x * w / 2.f;

            float y_bottom = sinf(angle_bottom);
            float vy_bottom = y_bottom * bridgeArcHeight;

            float vy_top = b/a * sqrtf(a*a - vx*vx);

            float vy_under = (float) i * (vy_top - vy_bottom) / bridgeSubsV + vy_bottom;
            float y_under = vy_under / bridgeArcHeight;

            float vy_over = (float) (i+1) * (vy_top - vy_bottom) / bridgeSubsV + vy_bottom;
            float y_over = vy_over / bridgeArcHeight;

            float angle_under = atan2f(y_under, x);
            float angle_over = atan2f(y_over, x);

            float radius_under = sqrtf(x*x + y_under*y_under);
            float radius_over = sqrtf(x*x + y_over*y_over);

            v1.position.x = v2.position.x = vx;
            v1.position.y = vy_under;
            v2.position.y = vy_over;
            v1.position.z = v2.position.z = z;

            v1.texcoords.x = - angle_under * bridgeWallHeight / bridgeArcHeight / M_PI / texSize;
            v2.texcoords.x = - angle_over * bridgeWallHeight / bridgeArcHeight / M_PI / texSize;
            v1.texcoords.y = 1.f - (radius_under - 1.f) * bridgeArcHeight / texSize;
            v2.texcoords.y = 1.f - (radius_over - 1.f) * bridgeArcHeight / texSize;

            vertices.push_back(v1);
            vertices.push_back(v2);

            addIndices(vertices, indices, startIndex, true);
        }
    }
}

void bridge::drawTopWall(std::vector<vertex> &vertices, std::vector<GLuint> &indices, float z1, float z2, float w, float texSize) {
    GLuint startIndex = (GLuint)vertices.size();

    glm::vec3 position;
    float maxV = abs(z1 - z2) / texSize;

	float angle;

    float a = bridgeWallHeight / bridgeArcHeight * abs(w / 2.f);
    float b = bridgeWallHeight;

	for (int i=0; i <= bridgeSubsH; ++i) {
        angle = M_PI / bridgeSubsH * i;

        float x = cosf(angle) * w / 2.f;
        position.x = x;
        position.y = b / a * sqrtf(a*a - x*x);

        float u = angle * bridgeArcHeight / texSize;

        vertex v1 = {{position.x, position.y, MAX(z1, z2)}, {u, 0}};
        vertex v2 = {{position.x, position.y, MIN(z1, z2)}, {u, maxV}};

        vertices.push_back(v1);
        vertices.push_back(v2);
	}

	addIndices(vertices, indices, startIndex, true);
}

void bridge::addIndices(std::vector<vertex> &vertices, std::vector<GLuint> &indices, GLuint startIndex, bool ext) {
	for (GLuint i = startIndex; i < vertices.size() - 2; i += 2) {
        if (ext) {
            indices.push_back(i);
            indices.push_back(i+1);
            indices.push_back(i+2);
            indices.push_back(i+2);
            indices.push_back(i+1);
            indices.push_back(i+3);
        } else {
            indices.push_back(i);
            indices.push_back(i+2);
            indices.push_back(i+1);
            indices.push_back(i+1);
            indices.push_back(i+2);
            indices.push_back(i+3);
        }
	}
}
