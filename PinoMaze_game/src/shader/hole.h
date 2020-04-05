#ifndef __SHADER_HOLE_H__
#define __SHADER_HOLE_H__

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "maze.h"

#include "../engine/shader_program.h"
#include "../engine/framebuffer.h"

class holeShader: public shaderProgram {
public:
    holeShader() : shaderProgram("hole") {}

public:
    bool init();
	
	bool bindProgram();
	void unbindProgram();

public:
    void setProjectionMatrix(const glm::mat4 &m) {
        setUniform(projectionMatrix, m);
    }
    void setViewMatrix(const glm::mat4 &m) {
        setUniform(viewMatrix, m);
    }
	void setLightDirection(const glm::vec3 &v) {
		setUniform(lightDirection, v);
	}
    void setRefractionTexture(const unsigned int &i) {
        setUniform(refractionTexture, i);
    }
    void setDudvTexture(const unsigned int &i) {
        setUniform(dudvTexture, i);
    }
	void setNormalTexture(const unsigned int &i) {
		setUniform(normalTexture, i);
	}
    void setTickCount(const int &i) {
        setUniform(tickCount, i);
    }

private:
    GLint projectionMatrix;
    GLint viewMatrix;

	GLint lightDirection;

    GLint refractionTexture;
    GLint dudvTexture;
	GLint normalTexture;

    GLint tickCount;

protected:
    void bindAddresses();
};

class holeRenderer {
protected:
    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    GLuint matrixBuffer = 0;

    holeShader shader;

private:
    maze *m = nullptr;

	int tickCount = 0;

    std::vector<glm::mat4> matrices;

	texture TEX_WATER_DUDV;
	texture TEX_WATER_NORMALS;

	texture refraction;
	texture refractionDepth;
	framebuffer refractionFBO;

public:
	holeRenderer(maze *m);
    virtual ~holeRenderer();

public:
	void bindFramebuffer() {
		return refractionFBO.bindFramebuffer();
	}

    bool init();

	void tick(class game *g);
    void render(class game *g);
};

#endif // __SHADER_HOLE_H__
