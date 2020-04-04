#include "shader_skybox.h"

#include "resources.h"

void skyboxShader::setViewMatrix(glm::mat4 m) {
    m[3][0] = 0.f;
    m[3][1] = 0.f;
    m[3][2] = 0.f;
    setUniform(viewMatrix, m);
}

bool skyboxShader::init() {
    return shaderProgram::loadProgramFromResource("IDS_SKYBOX_VERTEX", "IDS_SKYBOX_FRAGMENT");
}

bool skyboxShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;
	
	glEnableVertexAttribArray(0);

	return true;
}

void skyboxShader::unbindProgram() {
	glDisableVertexAttribArray(0);
	
	shaderProgram::unbindProgram();
}

void skyboxShader::bindAddresses() {
    projectionMatrix = glGetUniformLocation(programID, "projectionMatrix");
    viewMatrix = glGetUniformLocation(programID, "viewMatrix");
    cubeMap = glGetUniformLocation(programID, "cubeMap");
}
