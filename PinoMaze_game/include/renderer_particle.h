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
	virtual ~particleRenderer();

public:
	bool init();
	virtual void clean();

	void tick(class game *g);
	void render(class game *g);
};

#endif