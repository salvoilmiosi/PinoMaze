#ifndef __RENDERER_PARTICLE_H__
#define __RENDERER_PARTICLE_H__

#include "particle_system.h"
#include "shader_particle.h"

class particleRenderer {
protected:
	particleShader shader;

private:
	particleSystem system;
	texture TEX_PARTICLE_TEXTURE;

public:
	bool init();

	void tick(class game *g);
	void render(class game *g);
};

#endif