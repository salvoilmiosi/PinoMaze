#include "hole.h"

#include "../game.h"
#include "../globals.h"
#include "../resources.h"


holeRenderer::holeRenderer(maze *m) : shaderProgram("hole"), m(m) {
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

    if (!loadProgramFromResource(SHADER_RESOURCE(s_hole_v), SHADER_RESOURCE(s_hole_f))) return false;

	TEX_WATER_DUDV.loadSurface(loadImageFromResource("IDT_WATER_DUDV"));
	TEX_WATER_NORMALS.loadSurface(loadImageFromResource("IDT_WATER_NORMALS"));

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

	if (bindProgram()) {
		setRefractionTexture(0);
		setDudvTexture(1);
		setNormalTexture(2);
		unbindProgram();
	} else {
		return false;
	}

	return checkGlError("Failed to init hole model");
}

void holeRenderer::tick(game *g) {
	++tickCount;
}

void holeRenderer::render(game *g) {
	glBindVertexArray(vertexArray);
	if (bindProgram()) {
		setProjectionMatrix(g->projectionMatrix());
		setViewMatrix(g->viewMatrix());
		setTickCount(tickCount);

		refraction.bindTexture(0);
		TEX_WATER_DUDV.bindTexture(1);
		TEX_WATER_NORMALS.bindTexture(2);

		setLightDirection(g->worldLight().direction);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (int)matrices.size());

		unbindProgram();
	}
	glBindVertexArray(0);
}

bool holeRenderer::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	return true;
}

void holeRenderer::unbindProgram() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	
	shaderProgram::unbindProgram();
}

void holeRenderer::bindAddresses() {
    loc_projectionMatrix = glGetUniformLocation(programID, "projectionMatrix");
    loc_viewMatrix = glGetUniformLocation(programID, "viewMatrix");

	loc_lightDirection = glGetUniformLocation(programID, "lightDirection");

    loc_refractionTexture = glGetUniformLocation(programID, "refractionTexture");
    loc_dudvTexture = glGetUniformLocation(programID, "dudvTexture");
	loc_normalTexture = glGetUniformLocation(programID, "normalTexture");

    tickCount = glGetUniformLocation(programID, "tickCount");
}