#include "renderer_hole.h"

#include "res_loader.h"
#include "game.h"
#include "globals.h"

holeRenderer::holeRenderer(maze *m) : m(m) {
    glm::mat4 matrix, identity;

    for (int i=0; i<m->datasize(); ++i) {
        tile *t = m->getTile(i);
        int x = i % m->width();
        int y = i / m->width();
        if (t->state == STATE_BLOCK) {
            matrix = glm::translate(identity, glm::vec3(x * tileSize, -blockHeight, y * tileSize));
            matrices.push_back(matrix);
        }
    }
}

holeRenderer::~holeRenderer() {
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &matrixBuffer);

	glBindVertexArray(vertexArray);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDeleteVertexArrays(1, &vertexArray);
}

bool holeRenderer::init() {
	refraction.setFilter(GL_NEAREST);
	refraction.setWrapParam(GL_CLAMP_TO_EDGE);
	if (!refraction.createEmpty(windowWidth, windowHeight)) return false;

	refractionDepth.setFilter(GL_NEAREST);
	refractionDepth.setWrapParam(GL_CLAMP_TO_EDGE);
	if (!refractionDepth.createEmpty(windowWidth, windowHeight, true)) return false;

	if (!refractionFBO.init()) return false;
	refractionFBO.attachTexture(refraction);
	refractionFBO.attachDepthMap(refractionDepth);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

	if (!shader.init()) return false;

	TEX_WATER_DUDV.loadSurface(loadImageFromResources(IDT_WATER_DUDV));
	TEX_WATER_NORMALS.loadSurface(loadImageFromResources(IDT_WATER_NORMALS));

    const glm::vec3 vertices[] = {
        glm::vec3(0.0, 0.f, 0.0),
        glm::vec3(0.0, 0.f, tileSize),
        glm::vec3(tileSize, 0.f, 0.0),
        glm::vec3(tileSize, 0.f, tileSize),
    };

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);

    glGenBuffers(1, &matrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * matrices.size(), matrices.data(), GL_STATIC_DRAW);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(glm::mat4), 0);
    glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(4*sizeof(float)));
    glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(8*sizeof(float)));
    glVertexAttribPointer(4, 4, GL_FLOAT, false, sizeof(glm::mat4), (void*)(12*sizeof(float)));

	if (shader.bindProgram()) {
		shader.setRefractionTexture(0);
		shader.setDudvTexture(1);
		shader.setNormalTexture(2);
		shader.unbindProgram();
	} else {
		return false;
	}

    return glGetError() == GL_NO_ERROR;
}

void holeRenderer::tick(game *g) {
	++tickCount;
}

void holeRenderer::render(game *g) {
	glBindVertexArray(vertexArray);
	if (shader.bindProgram()) {
		shader.setProjectionMatrix(g->projectionMatrix());
		shader.setViewMatrix(g->viewMatrix());
		shader.setTickCount(tickCount);

		refraction.bindTexture(0);
		TEX_WATER_DUDV.bindTexture(1);
		TEX_WATER_NORMALS.bindTexture(2);

		shader.setLightDirection(g->worldLight().direction);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (int)matrices.size());

		shader.unbindProgram();
	}
	glBindVertexArray(0);
}
