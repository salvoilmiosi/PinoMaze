#ifndef __MODEL_HOLE_H__
#define __MODEL_HOLE_H__

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace std;

#include "shader_hole.h"
#include "framebuffer.h"
#include "maze.h"

class holeRenderer {
protected:
    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    GLuint matrixBuffer = 0;

    holeShader shader;

private:
    maze *m = nullptr;

	int tickCount = 0;

    vector<glm::mat4> matrices;

	texture TEX_WATER_DUDV;
	texture TEX_WATER_NORMALS;

	texture refraction;
	texture refractionDepth;
	framebuffer refractionFBO;

public:
    virtual ~holeRenderer();

public:
    void setMaze(maze *m);

	void bindFramebuffer() {
		return refractionFBO.bindFramebuffer();
	}

    bool init();
    virtual void clean();

	void tick(class game *g);
    void render(class game *g);
};

#endif // __MODEL_HOLE_H__
