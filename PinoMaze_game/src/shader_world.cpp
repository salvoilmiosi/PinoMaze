#include "shader_world.h"

#include "res_loader.h"

bool worldShader::init() {
    return shaderProgram::loadProgramFromResource(IDS_WORLD_VERTEX, IDS_WORLD_FRAGMENT);
}

bool worldShader::bindProgram() {
	if (!shaderProgram::bindProgram()) return false;
	
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);

	return true;
}

void worldShader::unbindProgram() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(7);
	
	shaderProgram::unbindProgram();
}

void worldShader::setLight(const light &l) {
    setUniform(sunAmbient, l.ambient);
    setUniform(sunDiffuse, l.diffuse);
    setUniform(sunSpecular, l.specular);
    setUniform(sunDirection, l.direction);
}

void worldShader::setMaterial(const material &m) {
    setUniform(matAmbient, m.ambient);
    setUniform(matDiffuse, m.diffuse);
    setUniform(matSpecular, m.specular);
    setUniform(matEmissive, m.emissive);
    setUniform(matShininess, m.shininess);
    if (m.tex) {
        m.tex->bindTexture(diffuseSampler);
        setUniform(enableTexture, true);
    } else {
        setUniform(enableTexture, false);
    }
    if (m.normals) {
        m.normals->bindTexture(normalSampler);
        setUniform(enableNormalMap, true);
    } else {
        setUniform(enableNormalMap, false);
    }
}

void worldShader::bindAddresses() {
    projectionMatrix = glGetUniformLocation(programID, "projectionMatrix");
    viewMatrix = glGetUniformLocation(programID, "viewMatrix");

    lightMatrix = glGetUniformLocation(programID, "lightMatrix");

    diffuseTexture = glGetUniformLocation(programID, "diffuseTexture");
    normalTexture = glGetUniformLocation(programID, "normalTexture");
    shadowMap = glGetUniformLocation(programID, "shadowMap");
    enableTexture = glGetUniformLocation(programID, "enableTexture");
    enableNormalMap = glGetUniformLocation(programID, "enableNormalMap");

    sunAmbient = glGetUniformLocation(programID, "sun.ambient");
    sunDiffuse = glGetUniformLocation(programID, "sun.diffuse");
    sunSpecular = glGetUniformLocation(programID, "sun.specular");
    sunDirection = glGetUniformLocation(programID, "sun.direction");

    matAmbient = glGetUniformLocation(programID, "mat.ambient");
    matDiffuse = glGetUniformLocation(programID, "mat.diffuse");
    matSpecular = glGetUniformLocation(programID, "mat.specular");
    matEmissive = glGetUniformLocation(programID, "mat.emissive");
    matShininess = glGetUniformLocation(programID, "mat.shininess");

    shadowBias = glGetUniformLocation(programID, "shadowBias");
    shadowTexelSize = glGetUniformLocation(programID, "shadowTexelSize");

	refractionHeight = glGetUniformLocation(programID, "refractionHeight");
}
