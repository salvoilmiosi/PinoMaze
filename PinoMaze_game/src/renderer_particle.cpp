#include "renderer_particle.h"

#include "res_loader.h"
#include "game.h"

particleRenderer::~particleRenderer() {
	clean();
}

bool particleRenderer::init() {
	if (!shader.init()) return false;

	if (!system.init()) return false;

	TEX_PARTICLE_TEXTURE.loadSurface(loadImageFromResources(IDT_PARTICLE_TEXTURE));

	if (shader.bindProgram()) {
		shader.setTexSize(0.5f);
		shader.setParticleTexture(0);
		shader.unbindProgram();
	} else {
		return false;
	}

	return glGetError() == GL_NO_ERROR;
}

void particleRenderer::clean() {
	shader.clean();

	system.clean();
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
		system.addParticles(600);
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