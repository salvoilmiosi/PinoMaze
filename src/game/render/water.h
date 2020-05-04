#ifndef __WATER_H__
#define __WATER_H__

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../engine/shader.h"
#include "../engine/vertex_array.h"
#include "../engine/framebuffer.h"

class water {
public:
	water(context *con, class game *m_game);

public:
	void bindRefraction() {
		refractionFBO.bind();
	}

    void load_models(int gridx, int gridy, int gridsize);

    void render(float deltaNano);
    
private:
	class game *m_game;

    vf_shader m_shader;
	vertex_array vao{DRAW_TRIANGLE_STRIP};

	texture refraction;
	texture refractionDepth;
	framebuffer refractionFBO;

    sampler refractionSampler = 0;
    sampler dudvSampler = 1;
    sampler normalSampler = 2;

	float shininess = 10.f;
	float globalTime = 0.f;
};

#endif // __WATER_H__
