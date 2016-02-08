#include "shader_hole.h"

#include "res_loader.h"

bool holeShader::init() {
    return shaderProgram::loadProgramFromResource("IDS_HOLE_VERTEX", "IDS_HOLE_FRAGMENT");
}

bool holeShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	return true;
}

void holeShader::unbindProgram() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	
	shaderProgram::unbindProgram();
}

void holeShader::bindAddresses() {
    projectionMatrix = glGetUniformLocation(programID, "projectionMatrix");
    viewMatrix = glGetUniformLocation(programID, "viewMatrix");

	lightDirection = glGetUniformLocation(programID, "lightDirection");

    refractionTexture = glGetUniformLocation(programID, "refractionTexture");
    dudvTexture = glGetUniformLocation(programID, "dudvTexture");
	normalTexture = glGetUniformLocation(programID, "normalTexture");

    tickCount = glGetUniformLocation(programID, "tickCount");
}

