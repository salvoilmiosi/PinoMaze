#ifndef __SIMPLE_SHADER_H__
#define __SIMPLE_SHADER_H__

#include "shader_program.h"
#include "material.h"

struct light {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 direction;
};

class worldShader: public shaderProgram {
public:
    worldShader() : shaderProgram("world") {}

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
    void setLightMatrix(const glm::mat4 &m) {
        setUniform(lightMatrix, m);
    }
    void setTextureSampler(const unsigned int &i) {
        setUniform(diffuseTexture, i);
		diffuseSampler = i;
    }
    void setNormalSampler(const unsigned int &i) {
        setUniform(normalTexture, i);
		normalSampler = i;
    }
    void setShadowSampler(const unsigned int &i) {
        setUniform(shadowMap, i);
    }
    void setShadowBias(const float &f) {
        setUniform(shadowBias, f);
    }
    void setShadowTexelSize(const glm::vec2 &v) {
        setUniform(shadowTexelSize, v);
    }
	void setRefractionHeight(const float &f) {
		setUniform(refractionHeight, f);
	}

    void setLight(const light &l);
    void setMaterial(const material &m);

private:
	int diffuseSampler;
	int normalSampler;

    GLint projectionMatrix;
    GLint viewMatrix;

    GLint lightMatrix;

    GLint diffuseTexture;
    GLint normalTexture;
    GLint shadowMap;

    GLint enableTexture;
    GLint enableNormalMap;

    GLint sunAmbient;
    GLint sunDiffuse;
    GLint sunSpecular;
    GLint sunDirection;

    GLint matAmbient;
    GLint matDiffuse;
    GLint matSpecular;
    GLint matEmissive;
    GLint matShininess;

    GLint shadowBias;
    GLint shadowTexelSize;

	GLint refractionHeight;

protected:
    void bindAddresses();
};

#endif // __SIMPLE_SHADER_H__
