#include "renderer_bridge.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <cstdio>

#include "texture.h"
#include "res_loader.h"
#include "globals.h"

bridgeRenderer::~bridgeRenderer() {
	clean();
}

void bridgeRenderer::setMaze(maze *_m) {
    m = _m;

    glm::mat4 identity, matrix;
    bool wallUp, wallDown;

    for (pair<const int, mazeItem> &it : m->items) {
        if (it.second.type == ITEM_BRIDGE) {
            int x = it.second.item.x;
            int y = it.second.item.y;

            matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, 0.f, (y + 0.5f) * tileSize));
            wallUp = ! m->hwalls[y][x];
            wallDown = ! m->hwalls[y+1][x];

            tile *tileDown = m->getTile(x, y+1);
            mazeItem *item;
            if (tileDown && (item = m->findItem(tileDown)) && item->type == ITEM_BRIDGE) {
                wallDown = false;
            }

            addMatrix(matrix);
            if (wallUp) wallMatrices.push_back(glm::translate(matrix, glm::vec3(0.f, 0.f, -tileSize * 0.5f)));
            if (wallDown) wallMatrices.push_back(glm::translate(matrix, glm::vec3(0.f, 0.f, tileSize * 0.5f)));
        }
    }
}

bool bridgeRenderer::init() {
    float l = tileSize - wallThickness;
    float l2 = l + bridgeArcThickness;
    float h2 = l2 / l * bridgeArcHeight;

    overArc_offset = indices.size();
    drawArc(-l/2.f, l/2.f, l2, h2, tileSize / 1.5f, true);

    underArc_offset = indices.size();
    drawArc(-l/2.f, l/2.f, l, bridgeArcHeight, l, false);

    wall_offset = indices.size();
	drawArc(wallThickness / 2.f, -wallThickness / 2.f, l, bridgeArcHeight, l / 1.5f, false);
    drawTopWall(wallThickness / 2.f, -wallThickness /2.f, l, l / 1.5f);

    drawArcWall(-wallThickness / 2.f, -l, tileSize / 1.5f);
    drawArcWall(wallThickness / 2.f, l, tileSize / 1.5f);

    index_count = indices.size();

    if (!createBuffers(vertices.data(), indices.data(), vertices.size(), indices.size())) return false;
    if (!calculateNormals(vertices.data(), indices.data(), vertices.size(), indices.size())) return false;
    if (!calculateTangents(vertices.data(), indices.data(), vertices.size(), indices.size())) return false;

    if (!createMatrixBuffer(matrices.data(), matrices.size())) return false;
    if (!loadToVAO(true)) return false;

    arcVAO = vertexArray;
    arcMatrixVBO = matrixBuffer;

    if (!createMatrixBuffer(wallMatrices.data(), wallMatrices.size())) return false;
    if (!loadToVAO(true)) return false;

    wallVAO = vertexArray;
    wallMatrixVBO = matrixBuffer;

    return true;
}

void bridgeRenderer::clean() {
    vertexArray = 0;
    matrixBuffer = 0;

    model::clean();
    glDeleteVertexArrays(1, &arcVAO);
    glDeleteVertexArrays(1, &wallVAO);

    glDeleteBuffers(1, &arcMatrixVBO);
    glDeleteBuffers(1, &wallMatrixVBO);
}

void bridgeRenderer::render() {
    if (!matrices.empty()) {
        vertexArray = arcVAO;
        bindModel();

        glDrawElementsInstanced(GL_TRIANGLES, GLsizei(wall_offset - overArc_offset), GL_UNSIGNED_INT,
                                (void*)(overArc_offset * sizeof(GLuint)), (GLsizei)matrices.size());

        unbindModel();
    }

    if (!wallMatrices.empty()) {
        vertexArray = wallVAO;
        bindModel();

        glDrawElementsInstanced(GL_TRIANGLES, GLsizei(index_count - wall_offset), GL_UNSIGNED_INT,
                                (void*)(wall_offset * sizeof(GLuint)), (GLsizei)wallMatrices.size());
        unbindModel();
    }

}

void bridgeRenderer::renderShader(worldShader &shader) {
    if (!matrices.empty()) {
        vertexArray = arcVAO;
        bindModel();

        shader.setMaterial(material::MAT_TILES);

        glDrawElementsInstanced(GL_TRIANGLES, GLsizei(underArc_offset - overArc_offset), GL_UNSIGNED_INT,
                                (void*)(overArc_offset * sizeof(GLuint)), (GLsizei)matrices.size());

        shader.setMaterial(material::MAT_PLASTER);

        glDrawElementsInstanced(GL_TRIANGLES, GLsizei(wall_offset - underArc_offset), GL_UNSIGNED_INT,
                                (void*)(underArc_offset * sizeof(GLuint)), (GLsizei)matrices.size());

        unbindModel();
    }

    if (!wallMatrices.empty()) {
        shader.setMaterial(material::MAT_BRICKS);

        vertexArray = wallVAO;
        bindModel();

        glDrawElementsInstanced(GL_TRIANGLES, GLsizei(index_count - wall_offset), GL_UNSIGNED_INT,
                                (void*)(wall_offset * sizeof(GLuint)), (GLsizei)wallMatrices.size());

        unbindModel();
    }
}

void bridgeRenderer::drawArc(float z1, float z2, float w, float h, float texSize, bool ext) {
    GLuint startIndex = (GLuint)vertices.size();

    glm::vec3 position;

	float maxV = abs(z1 - z2) / texSize;

	float angle;

	for (int i=0; i <= bridgeSubsH; ++i) {
        angle = (float) M_PI / bridgeSubsH * i;

		position = {cosf(angle) * w / 2.f, sinf(angle) * h, 0.f};

        float u = angle * bridgeArcHeight / texSize;

        vertex v1 = {position.x, position.y, MAX(z1, z2), u, 0};
        vertex v2 = {position.x, position.y, MIN(z1, z2), u, maxV};

        vertices.push_back(v1);
        vertices.push_back(v2);
	}

	addIndices(startIndex, ext);
}

void bridgeRenderer::drawArcWall(float z, float w, float texSize) {
    float a = bridgeWallHeight / bridgeArcHeight * abs(w / 2.f);
    float b = bridgeWallHeight;

    vertex v1, v2;
    for (int i=0; i<bridgeSubsV; ++i) {
        GLuint startIndex = (GLuint)vertices.size();
        for (int j=0; j<=bridgeSubsH; ++j) {
            float angle_bottom = (float) M_PI * j / bridgeSubsH;

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

            v1.x = v2.x = vx;
            v1.y = vy_under;
            v2.y = vy_over;
            v1.z = v2.z = z;

            v1.u = - angle_under * bridgeWallHeight / bridgeArcHeight / (float) M_PI / texSize;
            v2.u = - angle_over * bridgeWallHeight / bridgeArcHeight / (float) M_PI / texSize;
            v1.v = 1.f - (radius_under - 1.f) * bridgeArcHeight / texSize;
            v2.v = 1.f - (radius_over - 1.f) * bridgeArcHeight / texSize;

            vertices.push_back(v1);
            vertices.push_back(v2);

            addIndices(startIndex, true);
        }
    }
}

void bridgeRenderer::drawTopWall(float z1, float z2, float w, float texSize) {
    GLuint startIndex = (GLuint)vertices.size();

    glm::vec3 position;
    float maxV = abs(z1 - z2) / texSize;

	float angle;

    float a = bridgeWallHeight / bridgeArcHeight * abs(w / 2.f);
    float b = bridgeWallHeight;

	for (int i=0; i <= bridgeSubsH; ++i) {
        angle = (float) M_PI / bridgeSubsH * i;

        float x = cosf(angle) * w / 2.f;
        position.x = x;
        position.y = b / a * sqrtf(a*a - x*x);

        float u = angle * bridgeArcHeight / texSize;

        vertex v1 = {position.x, position.y, MAX(z1, z2), u, 0};
        vertex v2 = {position.x, position.y, MIN(z1, z2), u, maxV};

        vertices.push_back(v1);
        vertices.push_back(v2);
	}

	addIndices(startIndex, true);
}

void bridgeRenderer::addIndices(GLuint startIndex, bool ext) {
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
