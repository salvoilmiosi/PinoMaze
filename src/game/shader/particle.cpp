#include "particle.h"

#include "../resources.h"
#include "../game.h"
#include "../globals.h"

bool particleShader::init() {
	return shaderProgram::loadProgramFromResource("IDS_PARTICLE_VERTEX", "IDS_PARTICLE_FRAGMENT");
}

bool particleShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	return true;
}

void particleShader::unbindProgram() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	shaderProgram::unbindProgram();
}

void particleShader::bindAddresses() {
	projectionMatrix = glGetUniformLocation(programID, "projectionMatrix");
	viewMatrix = glGetUniformLocation(programID, "viewMatrix");

	texSize = glGetUniformLocation(programID, "texSize");
	particleTexture = glGetUniformLocation(programID, "particleTexture");
}

bool particleRenderer::init() {
	if (!shader.init()) return false;

	if (!system.init()) return false;

	TEX_PARTICLE_TEXTURE.loadSurface(loadImageFromResource("IDT_PARTICLE_TEXTURE"));

	if (shader.bindProgram()) {
		shader.setTexSize(0.5f);
		shader.setParticleTexture(0);
		shader.unbindProgram();
	} else {
		return false;
	}

	return checkGlError("Failed to init particle system renderer");
}

void particleRenderer::tick(game *g) {
	glm::mat4 matrix = g->marbleMatrix();
	glm::vec3 position(matrix[3][0], matrix[3][1], matrix[3][2]);
	static glm::vec3 lastPosition = position;
	system.setPosition(position);
	system.setVelocity(position - lastPosition);
	lastPosition = position;
	static int enableTimer = 0;

	if (g->hasTeleported()) {
		system.setVelocity(glm::vec3(0.f));
		system.addParticles(600, 1.5f);
		enableTimer = 20;
	}

	if (enableTimer > 0) --enableTimer;

	system.setEnabled(g->hasWon() || enableTimer > 0);

	system.tick();
}

void particleRenderer::render(game *g) {
	if (shader.bindProgram()) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(false);

		shader.setProjectionMatrix(g->projectionMatrix());
		shader.setViewMatrix(g->viewMatrix());

		TEX_PARTICLE_TEXTURE.bindTexture(0);

		system.render();

		glDepthMask(true);
		glDisable(GL_BLEND);
		shader.unbindProgram();
	}
}