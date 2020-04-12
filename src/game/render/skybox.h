#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include "../engine/vertex_array.h"
#include "../engine/cubemap.h"
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

	cubemap CUB_SKYBOX;

    glm::mat4 m_view_zeroed;
    int cubeMapSampler = 0;
};

#endif // __SKYBOX_H__
