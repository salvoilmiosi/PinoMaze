#include "renderer_skybox.h"

#include "res_loader.h"
#include "game.h"
#include "globals.h"

skyboxRenderer::~skyboxRenderer() {
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}

bool skyboxRenderer::init() {
    if (!shader.init()) return false;

	SDL_Surface *skyboxSurfaces[] = {
		loadImageFromResources(IDC_SKYBOX_LEFT),
		loadImageFromResources(IDC_SKYBOX_RIGHT),
		loadImageFromResources(IDC_SKYBOX_TOP),
		loadImageFromResources(IDC_SKYBOX_BOTTOM),
		loadImageFromResources(IDC_SKYBOX_BACK),
		loadImageFromResources(IDC_SKYBOX_FRONT),
	};

	CUB_SKYBOX.setWrapParam(GL_CLAMP_TO_EDGE);
	CUB_SKYBOX.loadSurfaces(skyboxSurfaces);

    glm::vec3 vertices[] = {
        // Front face
        glm::vec3(-skyboxSize, skyboxSize, skyboxSize),
        glm::vec3( skyboxSize, skyboxSize, skyboxSize),
        glm::vec3(-skyboxSize,-skyboxSize, skyboxSize),
        glm::vec3( skyboxSize,-skyboxSize, skyboxSize),

        // Back face
        glm::vec3( skyboxSize, skyboxSize,-skyboxSize),
        glm::vec3(-skyboxSize, skyboxSize,-skyboxSize),
        glm::vec3( skyboxSize,-skyboxSize,-skyboxSize),
        glm::vec3(-skyboxSize,-skyboxSize,-skyboxSize),

        // Left face
        glm::vec3(-skyboxSize, skyboxSize,-skyboxSize),
        glm::vec3(-skyboxSize, skyboxSize, skyboxSize),
        glm::vec3(-skyboxSize,-skyboxSize,-skyboxSize),
        glm::vec3(-skyboxSize,-skyboxSize, skyboxSize),

        // Right face
        glm::vec3( skyboxSize, skyboxSize, skyboxSize),
        glm::vec3( skyboxSize, skyboxSize,-skyboxSize),
        glm::vec3( skyboxSize,-skyboxSize, skyboxSize),
        glm::vec3( skyboxSize,-skyboxSize,-skyboxSize),

        // Top face
        glm::vec3(-skyboxSize, skyboxSize,-skyboxSize),
        glm::vec3( skyboxSize, skyboxSize,-skyboxSize),
        glm::vec3(-skyboxSize, skyboxSize, skyboxSize),
        glm::vec3( skyboxSize, skyboxSize, skyboxSize),

        // Bottom face
        glm::vec3( skyboxSize,-skyboxSize,-skyboxSize),
        glm::vec3(-skyboxSize,-skyboxSize,-skyboxSize),
        glm::vec3( skyboxSize,-skyboxSize, skyboxSize),
        glm::vec3(-skyboxSize,-skyboxSize, skyboxSize),
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 1, 3,

        4, 5, 6,
        6, 5, 7,

        8, 9, 10,
        10, 9, 11,

        12, 13, 14,
        14, 13, 15,

        16, 17, 18,
        18, 17, 19,

        20, 21, 22,
        22, 21, 23,
	};

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    if (glGetError() != GL_NO_ERROR) return false;

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    if (glGetError() != GL_NO_ERROR) return false;

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

    glBindVertexArray(0);

    return glGetError() == GL_NO_ERROR;
}

void skyboxRenderer::render(game *g) {
	glBindVertexArray(vertexArray);
	if (shader.bindProgram()) {

		CUB_SKYBOX.bindTexture(0);
		shader.setCubeMap(0);

		shader.setProjectionMatrix(g->projectionMatrix());
		shader.setViewMatrix(g->viewMatrix());

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		shader.unbindProgram();
	}
	glBindVertexArray(0);
}
