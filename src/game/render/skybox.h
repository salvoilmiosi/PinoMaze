#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include "../engine/engine.h"
#include "../engine/vertex_array.h"
#include "../engine/texture.h"
#include "../engine/shader.h"

class skybox {
public:
    skybox(class game *m_game);

public:
    void render();

private:
    class game *m_game;

    vf_shader m_shader;
    vertex_array vao;

	texture m_texture[6];
    int m_sampler[6] = {0, 1, 2, 3, 4, 5};

    glm::mat4 m_view_zeroed;
};

#endif // __SKYBOX_H__
