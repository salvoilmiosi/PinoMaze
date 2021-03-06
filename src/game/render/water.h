#ifndef __WATER_H__
#define __WATER_H__

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../engine/engine.h"
#include "../engine/shader.h"
#include "../engine/vertex_array.h"
#include "../engine/framebuffer.h"

class water {
public:
	water(engine *m_engine, class game *m_game);

public:

	void bindRefraction() {
		refractionFBO.bind();
	}

    void load_models(int gridx, int gridy, int gridsize);

    void render(float deltaNano);
    
private:
	class game *m_game;

	const engine_options &options;

    vf_shader m_shader;
	vertex_array vao{DRAW_TRIANGLE_STRIP};

	texture refraction;
	texture refractionDepth;
	framebuffer refractionFBO;

    int refractionSampler = 0;
    int dudvSampler = 1;
    int normalSampler = 2;

	float shininess = 10.f;
	float globalTime = 0.f;
};

#endif // __WATER_H__
