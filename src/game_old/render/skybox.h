#ifndef __SHADER_SKYBOX_H__
#define __SHADER_SKYBOX_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include "../engine/cubemap.h"
#include "../engine/shader_program.h"

class skyboxRenderer : public shaderProgram {
protected:
    GLuint vertexArray = 0;
	
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;

private:
	cubemap CUB_SKYBOX;

public:
    skyboxRenderer() : shaderProgram("skybox") {}
    virtual ~skyboxRenderer();

public:
    bool init();

    void render(class game *g);

public:
    void setProjectionMatrix(const glm::mat4 &m) {
        setUniform(projectionMatrix, m);
    }

    void setViewMatrix(glm::mat4 m);

    void setCubeMap(const unsigned int &i) {
        setUniform(cubeMap, i);
    }

private:
    GLint projectionMatrix;
    GLint viewMatrix;

    GLint cubeMap;

protected:
    void bindAddresses();
};

#endif // __SHADER_SKYBOX_H__
