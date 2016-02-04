#ifndef __RENDERER_TELEPORT_H__
#define __RENDERER_TELEPORT_H__

#include "box.h"
#include "maze.h"

#include "shader_teleport.h"

class teleportRenderer: public box {
private:
	teleportShader shader;

	maze *m = nullptr;

	GLuint uvBuffer = 0;
	vector<glm::vec2> uvOffsets;

	texture TEX_TELEPORT_TEXTURE;

public:
	teleportRenderer();
	virtual ~teleportRenderer();

public:
	void setMaze(maze *m);

	bool init();
	virtual void clean();

	void render(class game *g);
};

#endif // __RENDERER_TELEPORT_H__
