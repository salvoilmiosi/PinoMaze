#include "shader_hud.h"

#include "resources.h"

bool hudShader::init() {
    return shaderProgram::loadProgramFromResource("IDS_HUD_VERTEX", "IDS_HUD_FRAGMENT");
}

bool hudShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return true;
}

void hudShader::unbindProgram() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	shaderProgram::unbindProgram();
}

void hudShader::bindAddresses() {
    offset = glGetUniformLocation(programID, "offset");
    diffuseTexture = glGetUniformLocation(programID, "diffuseTexture");
    diffuseColor = glGetUniformLocation(programID, "diffuseColor");
}
