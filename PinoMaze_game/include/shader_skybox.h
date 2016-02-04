#ifndef __SHADER_SKYBOX_H__
#define __SHADER_SKYBOX_H__

#include "shader_program.h"

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

#endif // __SHADER_SKYBOX_H__
