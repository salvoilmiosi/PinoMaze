#ifndef __MODEL_SKYBOX_H__
#define __MODEL_SKYBOX_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include "cubemap.h"
#include "res_loader.h"

#include "shader_skybox.h"

class skyboxRenderer {
protected:
    GLuint vertexArray = 0;
	
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;

    skyboxShader shader;

private:
	cubemap CUB_SKYBOX;

public:
    virtual ~skyboxRenderer();

public:
    bool init();

    void render(class game *g);
};

#endif // __MODEL_SKYBOX_H__
