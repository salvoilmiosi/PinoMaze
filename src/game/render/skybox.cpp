#include "skybox.h"

#include "../resources.h"
#include "../game.h"
#include "../options.h"

skybox::skybox(game *m_game) : m_game(m_game),
    m_shader("skybox", SHADER_RESOURCE(s_skybox_v), SHADER_RESOURCE(s_skybox_f),
        "projectionMatrix", &m_game->m_proj,
        "viewMatrix", &m_view_zeroed,
        "skyboxTexture[0]", &m_sampler[0],
        "skyboxTexture[1]", &m_sampler[1],
        "skyboxTexture[2]", &m_sampler[2],
        "skyboxTexture[3]", &m_sampler[3],
        "skyboxTexture[4]", &m_sampler[4],
        "skyboxTexture[5]", &m_sampler[5]),
        
    m_texture {
        loadImageFromResource("IDC_SKYBOX_FRONT"),
        loadImageFromResource("IDC_SKYBOX_BACK"),
        loadImageFromResource("IDC_SKYBOX_RIGHT"),
        loadImageFromResource("IDC_SKYBOX_LEFT"),
        loadImageFromResource("IDC_SKYBOX_TOP"),
        loadImageFromResource("IDC_SKYBOX_BOTTOM"),
    }
{
    for (size_t i=0; i<6; ++i) {
	    m_texture[i].setWrapParam(GL_CLAMP_TO_EDGE);
    }

    struct vertex {
        glm::vec3 position;
        glm::vec2 texCoords;
        float index;
    };

    vertex vertices[] = {
        // Front face
        {{-skyboxSize, skyboxSize, skyboxSize}, {1.f, 0.f}, 0.f},
        {{ skyboxSize, skyboxSize, skyboxSize}, {0.f, 0.f}, 0.f},
        {{-skyboxSize,-skyboxSize, skyboxSize}, {1.f, 1.f}, 0.f},
        {{ skyboxSize,-skyboxSize, skyboxSize}, {0.f, 1.f}, 0.f},

        // Back face
        {{ skyboxSize, skyboxSize,-skyboxSize}, {1.f, 0.f}, 1.f},
        {{-skyboxSize, skyboxSize,-skyboxSize}, {0.f, 0.f}, 1.f},
        {{ skyboxSize,-skyboxSize,-skyboxSize}, {1.f, 1.f}, 1.f},
        {{-skyboxSize,-skyboxSize,-skyboxSize}, {0.f, 1.f}, 1.f},

        // Left face
        {{-skyboxSize, skyboxSize,-skyboxSize}, {1.f, 0.f}, 2.f},
        {{-skyboxSize, skyboxSize, skyboxSize}, {0.f, 0.f}, 2.f},
        {{-skyboxSize,-skyboxSize,-skyboxSize}, {1.f, 1.f}, 2.f},
        {{-skyboxSize,-skyboxSize, skyboxSize}, {0.f, 1.f}, 2.f},

        // Right face
        {{ skyboxSize, skyboxSize, skyboxSize}, {1.f, 0.f}, 3.f},
        {{ skyboxSize, skyboxSize,-skyboxSize}, {0.f, 0.f}, 3.f},
        {{ skyboxSize,-skyboxSize, skyboxSize}, {1.f, 1.f}, 3.f},
        {{ skyboxSize,-skyboxSize,-skyboxSize}, {0.f, 1.f}, 3.f},

        // Top face
        {{-skyboxSize, skyboxSize,-skyboxSize}, {0.f, 1.f}, 4.f},
        {{ skyboxSize, skyboxSize,-skyboxSize}, {1.f, 1.f}, 4.f},
        {{-skyboxSize, skyboxSize, skyboxSize}, {0.f, 0.f}, 4.f},
        {{ skyboxSize, skyboxSize, skyboxSize}, {1.f, 0.f}, 4.f},

        // Bottom face
        {{ skyboxSize,-skyboxSize,-skyboxSize}, {1.f, 0.f}, 5.f},
        {{-skyboxSize,-skyboxSize,-skyboxSize}, {0.f, 0.f}, 5.f},
        {{ skyboxSize,-skyboxSize, skyboxSize}, {1.f, 1.f}, 5.f},
        {{-skyboxSize,-skyboxSize, skyboxSize}, {0.f, 1.f}, 5.f},
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

    vao.update_vertices(0, vertices, sizeof(vertices) / sizeof(vertex), {{0, ATTR_VEC3}, {1, ATTR_VEC2}, {2, ATTR_FLOAT}});
    vao.update_indices(indices, sizeof(indices) / sizeof(GLuint));

	checkGlError("Failed to init skybox");
}

void skybox::render() {
    m_view_zeroed = m_game->m_view;
    m_view_zeroed[3][0] = 0.f;
    m_view_zeroed[3][1] = 0.f;
    m_view_zeroed[3][2] = 0.f;

    for (size_t i=0; i<6; ++i) {
        m_sampler[i].bind(m_texture + i);
    }
    m_shader.use();

    vao.draw();
}