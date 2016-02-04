#ifndef __HOLE_SHADER_H__
#define __HOLE_SHADER_H__

#include "shader_program.h"

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

#endif // __HOLE_SHADER_H__

