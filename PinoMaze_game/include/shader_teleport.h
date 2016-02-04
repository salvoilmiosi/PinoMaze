#ifndef __SHADER_TELEPORT_H__
#define __SHADER_TELEPORT_H__

#include "shader_world.h"

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

#endif // __SHADER_TELEPORT_H__