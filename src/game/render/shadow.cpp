#include "shadow.h"

#include "../resources.h"

shadowShader::~shadowShader() {
	glDeleteVertexArrays(1, &vao);
}

bool shadowShader::init() {
    if (!shaderProgram::loadProgramFromResource(SHADER_RESOURCE(s_shadow_v), SHADER_RESOURCE(s_shadow_f))) return false;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glBindVertexArray(0);

	return true;
}

bool shadowShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;
	
	glBindVertexArray(vao);

	return true;
}

void shadowShader::unbindProgram() {
	glBindVertexArray(0);
	
	shaderProgram::unbindProgram();
}

void shadowShader::bindAddresses() {
    lightMatrix = glGetUniformLocation(programID, "lightMatrix");
}

