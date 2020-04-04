#include "particle_system.h"

#include "game.h"

particleSystem::particleSystem() {
	vertexArray = 0;
	vertexBuffer = 0;
	particleBuffer = 0;

	position = glm::vec3(0.f);
	enabled = false;
	numAlive = 0;
}

particleSystem::~particleSystem() {
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &particleBuffer);
}

bool particleSystem::init() {
	static const glm::vec2 vertices[] = {
		glm::vec2(-1.f,  1.f),
		glm::vec2(-1.f, -1.f),
		glm::vec2( 1.f,  1.f),
		glm::vec2( 1.f, -1.f),
	};

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(glm::vec2), 0);

	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(particle), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(particle), 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(particle), (void*)(sizeof(glm::vec3)));
	glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(particle), (void*)(2 * sizeof(glm::vec3)));
	glVertexAttribPointer(4, 1, GL_FLOAT, false, sizeof(particle), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindVertexArray(0);

	return checkGlError("Failed to init particle system");
}

void particleSystem::removeDeadParticles() {
	particle *iD = particles;
	particle *iA = particles + numAlive - 1;
	while (true) {
		while (true) {
			if (iD > iA) goto endOfLoop;
			if (iD->life <= 0) break;
			++iD;
		}
		while (true) {
			if (iA->life > 0) break;
			if (iA <= iD) goto endOfLoop;
			--iA;
		}
		swap(*iD, *iA);

		++iD;
		--iA;
	}
endOfLoop:
	numAlive = iD - particles;
}

void particleSystem::addParticles(int num, float force) {
	particle p;

	while (num > 0) {
		if (numAlive >= maxParticles) return;

		p.position = position;
		float posX = (rand() / (float)RAND_MAX - 0.5f) * 0.01f;
		float posY = (rand() / (float)RAND_MAX - 0.5f) * 0.01f;
		float posZ = (rand() / (float)RAND_MAX - 0.5f) * 0.01f;

		p.velocity = glm::vec3(posX, posY, posZ);
		p.position += glm::normalize(p.velocity) * marbleRadius;
		p.velocity *= force;
		p.velocity += velocity * 0.3f;

		float r = rand() / (float)RAND_MAX;
		float g = rand() / (float)RAND_MAX;
		float b = rand() / (float)RAND_MAX;

		p.color = glm::vec3(r, g, b);
		p.uv = glm::vec2((rand() % 2) * 0.5f, (rand() % 2) * 0.5f);

		p.life = rand() % 60 + 40;

		p.particleSize = rand() / (float)RAND_MAX * 0.06f;

		particles[numAlive] = p;
		++numAlive;

		--num;
	}
}

void particleSystem::tick() {
	if (enabled) {
		addParticles(rand() % 15 + 5);
	}
	
	for (size_t i = 0; i < numAlive; ++i) {
		particle &p = particles[i];

		if (p.life <= 0) continue;

		--p.life;

		p.position += p.velocity;
		p.velocity *= 0.985f;
		p.velocity.y -= 0.00005f;
		p.particleSize *= 0.98f;
	}

	removeDeadParticles();
}

void particleSystem::render() {
	if (numAlive <= 0) return;

	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(particle) * numAlive, particles);

	glBindVertexArray(vertexArray);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)numAlive);
	glBindVertexArray(0);
}