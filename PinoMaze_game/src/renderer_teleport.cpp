#include "renderer_teleport.h"

#include "globals.h"
#include "game.h"

#include <glm/gtc/matrix_transform.hpp>

teleportRenderer::teleportRenderer(maze *m) : m(m) {
	setSize(startBoxSize, startBoxHeight, startBoxSize, startBoxSize);

	glm::mat4 identity, matrix;

	glm::vec2 uv;
	for (pair<const int, mazeItem> &it : m->items) {
		if (it.second.type == ITEM_TELEPORT) {
			unsigned char c = it.second.teleport.tpChar;
			uv.x = (1.f / 16.f) * (c % 16);
			uv.y = (1.f / 16.f) * (c / 16);
			uvOffsets.push_back(uv);

			int x = it.second.item.x;
			int y = it.second.item.y;

			matrix = glm::translate(identity, glm::vec3((x + 0.5f) * tileSize, startBoxHeight / 2.f, (y + 0.5f) * tileSize));
			matrices.push_back(matrix);
		}
	}
}

teleportRenderer::~teleportRenderer() {
	glDeleteBuffers(1, &uvBuffer);
}

bool teleportRenderer::init() {
	TEX_TELEPORT_TEXTURE.setFilter(GL_NEAREST);
	TEX_TELEPORT_TEXTURE.loadSurface(loadImageFromResources(IDT_TELEPORT_TEXTURE));

	if (!box::init()) return false;
	if (!shader.init()) return false;
	
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

	glBufferData(GL_ARRAY_BUFFER, uvOffsets.size() * sizeof(glm::vec2), uvOffsets.data(), GL_STATIC_DRAW);

	bindModel();

	glEnableVertexAttribArray(8);

	glVertexAttribDivisor(8, 1);
	glVertexAttribPointer(8, 2, GL_FLOAT, false, 0, 0);
	unbindModel();

	if (shader.bindProgram()) {
		shader.setTextureSampler(0);
		shader.setNormalSampler(1);
		shader.setShadowSampler(2);
		shader.setTileTexture(3);
		shader.setTileSize(1.f / 16.f);
		shader.unbindProgram();
	} else {
		return false;
	}

	return glGetError() == GL_NO_ERROR;
}

void teleportRenderer::render(game *g) {
	if (shader.bindProgram()) {
		shader.setProjectionMatrix(g->projectionMatrix());
		shader.setViewMatrix(g->viewMatrix());
		shader.setLightMatrix(biasMatrix * g->lightMatrix());

		shader.setLight(g->viewLight());

		shader.setMaterial(g->MAT_RUST);
		TEX_TELEPORT_TEXTURE.bindTexture(3);
		box::render();

		shader.unbindProgram();
	}
}