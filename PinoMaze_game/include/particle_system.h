#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include <glm/glm.hpp>

#include "model.h"

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
};

#endif