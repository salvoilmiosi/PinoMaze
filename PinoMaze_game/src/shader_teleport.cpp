#include "shader_teleport.h"

#include "resources.h"

bool teleportShader::init() {
	return loadProgramFromResource("IDS_TELEPORT_VERTEX", "IDS_TELEPORT_FRAGMENT");
}

bool teleportShader::bindProgram() {
	if (!worldShader::bindProgram()) return false;

	glEnableVertexAttribArray(8);

	return true;
}

void teleportShader::unbindProgram() {
	glDisableVertexAttribArray(8);
	worldShader::unbindProgram();
}

void teleportShader::bindAddresses() {
	worldShader::bindAddresses();

	tileSize = glGetUniformLocation(shaderProgram::programID, "tileSize");
	tileTexture = glGetUniformLocation(shaderProgram::programID, "tileTexture");
}