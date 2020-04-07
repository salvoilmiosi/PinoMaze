#ifndef __SHADER_HOLE_H__
#define __SHADER_HOLE_H__

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "maze.h"

#include "../engine/shader_program.h"
#include "../engine/framebuffer.h"

class holeRenderer: public shaderProgram {
protected:
    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    GLuint matrixBuffer = 0;

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

private:
    bool bindProgram();
    void unbindProgram();

    virtual void bindAddresses();

    void setProjectionMatrix(const glm::mat4 &m) {
        setUniform(loc_projectionMatrix, m);
    }
    void setViewMatrix(const glm::mat4 &m) {
        setUniform(loc_viewMatrix, m);
    }
	void setLightDirection(const glm::vec3 &v) {
		setUniform(loc_lightDirection, v);
	}
    void setRefractionTexture(const unsigned int &i) {
        setUniform(loc_refractionTexture, i);
    }
    void setDudvTexture(const unsigned int &i) {
        setUniform(loc_dudvTexture, i);
    }
	void setNormalTexture(const unsigned int &i) {
		setUniform(loc_normalTexture, i);
	}
    void setTickCount(const int &i) {
        setUniform(loc_tickCount, i);
    }
    
private:
    GLint loc_projectionMatrix;
    GLint loc_viewMatrix;

	GLint loc_lightDirection;

    GLint loc_refractionTexture;
    GLint loc_dudvTexture;
	GLint loc_normalTexture;

    GLint loc_tickCount;
};

#endif // __SHADER_HOLE_H__
