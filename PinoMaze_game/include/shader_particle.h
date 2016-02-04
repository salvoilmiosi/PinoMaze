#ifndef __SHADER_PARTICLE_H__
#define __SHADER_PARTICLE_H__

#include "shader_program.h"

class particleShader : public shaderProgram {
public:
	particleShader() : shaderProgram("particle") {}

public:
	bool init();
	
	bool bindProgram();
	void unbindProgram();

public:
	void setParticleTexture(const unsigned int &i) {
		setUniform(particleTexture, i);
	}
	void setTexSize(const float &f) {
		setUniform(texSize, f);
	}
	void setProjectionMatrix(const glm::mat4 &m) {
		setUniform(projectionMatrix, m);
	}
	void setViewMatrix(const glm::mat4 &m) {
		setUniform(viewMatrix, m);
	}

private:
	GLint particleTexture;
	GLint texSize;

	GLint projectionMatrix;
	GLint viewMatrix;

protected:
	void bindAddresses();
};

#endif