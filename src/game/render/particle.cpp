#include "particle.h"

#include "../resources.h"
#include "../game.h"
#include "../options.h"

particle_system::particle_system(game *m_game) : 
	m_game(m_game),
	m_shader("particle", SHADER_RESOURCE(s_particle_v), SHADER_RESOURCE(s_particle_f)),
	vao(DRAW_TRIANGLE_STRIP)
{
	m_shader.add_uniform("projectionMatrix", &m_game->m_proj);
	m_shader.add_uniform("viewMatrix", &m_game->m_view);
	m_shader.add_uniform("texSize", &texSize);
	m_shader.add_uniform("particleTexture", &particleSampler.gl_samplerid);

	checkGlError("Failed to init particle system renderer");
	
	static const glm::vec2 vertices[] = {
		glm::vec2(-1.f,  1.f),
		glm::vec2(-1.f, -1.f),
		glm::vec2( 1.f,  1.f),
		glm::vec2( 1.f, -1.f),
	};

	vao.update_vertices(0, vertices, 4 * sizeof(glm::vec2), {{0, ATTR_VEC2}});

	checkGlError("Failed to init particle system");
}

void particle_system::tick() {
	glm::mat4 matrix = m_game->m_marble;
	glm::vec3 position(matrix[3][0], matrix[3][1], matrix[3][2]);
	static glm::vec3 lastPosition = position;
	setPosition(position);
	setVelocity(position - lastPosition);
	lastPosition = position;
	static int enableTimer = 0;

	if (m_game->teleportTimer >= 72) {
		setVelocity(glm::vec3(0.f));
		addParticles(600, 1.5f);
		enableTimer = 20;
	}

	if (enableTimer > 0) --enableTimer;

	if (m_game->won || enableTimer > 0) {
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

void particle_system::render() {
	return;
	if (numAlive <= 0) return;

	m_shader.use_program();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(false);

	particleSampler.bind(material::getTexture("TEX_PARTICLE_TEXTURE"));

	vao.update_instances(1, particles, sizeof(particle) * numAlive,
		{{1, ATTR_VEC3}, {2, ATTR_VEC3}, {3, ATTR_VEC2}, {4, ATTR_FLOAT}, {5, ATTR_VEC3}, {6, ATTR_INT}}, true);

	vao.draw_instances();

	glDepthMask(true);
	glDisable(GL_BLEND);
}

void particle_system::removeDeadParticles() {
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
		std::swap(*iD, *iA);

		++iD;
		--iA;
	}
endOfLoop:
	numAlive = iD - particles;
}

void particle_system::addParticles(int num, float force) {
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