#ifndef __HOLE_H__
#define __HOLE_H__

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "../engine/shader.h"
#include "../engine/vertex_array.h"
#include "../engine/framebuffer.h"

class hole {
public:
	hole(context *con, class game *m_game);

public:
	void bindRefraction() {
		refractionFBO.bind();
	}

    void init(class maze *m_maze);

    void render();
    
private:
    vf_shader m_shader;
	vertex_array vao{DRAW_TRIANGLE_STRIP};

	texture refraction;
	texture refractionDepth;
	framebuffer refractionFBO;

    sampler refractionSampler{0};
    sampler dudvSampler{1};
    sampler normalSampler{2};

	float shininess = 10.f;
	float globalTime;
};

#endif // __HOLE_H__
