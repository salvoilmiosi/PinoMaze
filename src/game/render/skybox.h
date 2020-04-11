#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include "../engine/model.h"
#include "../engine/vertex_array.h"
#include "../engine/cubemap.h"
#include "../engine/shader.h"

class skybox : public model {
public:
    skybox(class game *m_game);

public:
    void draw();

private:
    class game *m_game;

    shader m_shader;

	cubemap CUB_SKYBOX;

    glm::mat4 m_view_zeroed;
    int cubeMapSampler = 0;
};

#endif // __SKYBOX_H__
