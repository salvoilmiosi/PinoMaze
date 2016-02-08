#include "shader_particle.h"

#include "res_loader.h"

bool particleShader::init() {
	return shaderProgram::loadProgramFromResource("IDS_PARTICLE_VERTEX", "IDS_PARTICLE_FRAGMENT");
}

bool particleShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	return true;
}

void particleShader::unbindProgram() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	shaderProgram::unbindProgram();
}

void particleShader::bindAddresses() {
	projectionMatrix = glGetUniformLocation(programID, "projectionMatrix");
	viewMatrix = glGetUniformLocation(programID, "viewMatrix");

	texSize = glGetUniformLocation(programID, "texSize");
	particleTexture = glGetUniformLocation(programID, "particleTexture");
}