#include "skybox.h"

#include "../resources.h"
#include "../game.h"
#include "../options.h"

skybox::skybox(game *m_game) :
    m_game(m_game),
    m_shader("skybox", SHADER_RESOURCE(s_skybox_v), SHADER_RESOURCE(s_skybox_f))
{
    m_shader.add_uniform("projectionMatrix", &m_game->m_proj);
    m_shader.add_uniform("viewMatrix", &m_view_zeroed);
    m_shader.add_uniform("cubeMap", &cubeMapSampler);

	SDL_Surface *skyboxSurfaces[] = {
		loadImageFromResource("IDC_SKYBOX_LEFT"),
		loadImageFromResource("IDC_SKYBOX_RIGHT"),
		loadImageFromResource("IDC_SKYBOX_TOP"),
		loadImageFromResource("IDC_SKYBOX_BOTTOM"),
		loadImageFromResource("IDC_SKYBOX_BACK"),
		loadImageFromResource("IDC_SKYBOX_FRONT"),
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

    update_buffer(0, vertices, sizeof(vertices), {{0, ATTR_VEC3}});
    update_indices(indices, sizeof(indices)/sizeof(GLuint));

	checkGlError("Failed to init skybox");
}

void skybox::draw() {
    m_view_zeroed = m_game->m_view;
    m_view_zeroed[3][0] = 0.f;
    m_view_zeroed[3][1] = 0.f;
    m_view_zeroed[3][2] = 0.f;

	CUB_SKYBOX.bindTexture(0);
    m_shader.use_program();
    model::draw();
}