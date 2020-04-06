#ifndef __SHADER_TELEPORT_H__
#define __SHADER_TELEPORT_H__

#include "world.h"
#include "maze.h"

#include "../engine/box.h"

class teleportShader : public worldShader {
public:
	teleportShader() {
		programName = "teleport";
	}

public:
	bool init();

	bool bindProgram();
	void unbindProgram();

public:
	void setTileSize(const float &f) {
		setUniform(tileSize, f);
	}

	void setTileTexture(const unsigned int &i) {
		setUniform(tileTexture, i);
	}

private:
	GLint tileSize;
	GLint tileTexture;

protected:
	void bindAddresses();
};

class teleportRenderer: public box {
private:
	teleportShader shader;

	maze *m = nullptr;

	GLuint uvBuffer = 0;
	std::vector<glm::vec2> uvOffsets;

	texture TEX_TELEPORT_TEXTURE;

public:
	teleportRenderer(maze *m);
	virtual ~teleportRenderer();

public:
	bool init();

	void render(class game *g);
};

#endif // __SHADER_TELEPORT_H__