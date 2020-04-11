#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "../engine/shader.h"
#include "../engine/texture.h"
#include "../engine/model.h"

struct particle {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
	float particleSize;

	glm::vec3 velocity;
	int life;
};

class particle_system : public model {
public:
	particle_system(class game *m_game);

public:
	bool init();

	void setPosition(const glm::vec3 &pos) {
		position = pos;
	}

	void setVelocity(const glm::vec3 &vel) {
		velocity = vel;
	}

	void addParticles(int num, float force = 1.f);

	void tick();
	void draw();

private:
	void removeDeadParticles();

private:
	static const int maxParticles = 2000;

	class game *m_game;
	shader m_shader;

	particle particles[maxParticles];
	size_t numAlive = 0;

	glm::vec3 position{0.f};
	glm::vec3 velocity;
	
	sampler particleSampler{0};
	float texSize = 0.5f;
};

#endif