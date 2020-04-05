#ifndef __SHADER_SKYBOX_H__
#define __SHADER_SKYBOX_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include "../engine/cubemap.h"
#include "../engine/shader_program.h"

class skyboxShader: public shaderProgram {
public:
    skyboxShader() : shaderProgram("skybox") {}

public:
    bool init();
	
	bool bindProgram();
	void unbindProgram();

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

#endif // __SHADER_SKYBOX_H__
