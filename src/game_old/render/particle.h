#ifndef __SHADER_PARTICLE_H__
#define __SHADER_PARTICLE_H__

#include "../engine/shader_program.h"
#include "../engine/texture.h"

struct particle {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
	float particleSize;

	glm::vec3 velocity;
	int life;
};

class particleSystem {
private:
	static const int maxParticles = 2000;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint particleBuffer;

	particle particles[maxParticles];
	size_t numAlive;

	glm::vec3 position;
	glm::vec3 velocity;

	bool enabled;

public:
	particleSystem();
	virtual ~particleSystem();

public:
	bool init();

	void setEnabled(bool b) {
		enabled = b;
	}

	void setPosition(const glm::vec3 &pos) {
		position = pos;
	}

	void setVelocity(const glm::vec3 &vel) {
		velocity = vel;
	}

	void addParticles(int num, float force = 1.f);

	void tick();
	void render();

private:
	void removeDeadParticles();

	friend class particleRenderer;
};

class particleRenderer : public shaderProgram {
public:
	particleRenderer() : shaderProgram("particle") {}

private:
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

private:
	particleSystem system;
	texture TEX_PARTICLE_TEXTURE;

public:
	bool init();

	void tick(class game *g);
	void render(class game *g);
};

#endif