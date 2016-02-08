#include "shader_shadow.h"

#include "res_loader.h"

bool shadowShader::init() {
    return shaderProgram::loadProgramFromResource("IDS_SHADOW_VERTEX", "IDS_SHADOW_FRAGMENT");
}

bool shadowShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);

	return true;
}

void shadowShader::unbindProgram() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(6);
	glDisableVertexAttribArray(7);
	
	shaderProgram::unbindProgram();
}

void shadowShader::bindAddresses() {
    lightMatrix = glGetUniformLocation(programID, "lightMatrix");
}

