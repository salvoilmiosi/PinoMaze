#ifndef __SIMPLE_SHADER_H__
#define __SIMPLE_SHADER_H__

#include <memory>

#include "shader_program.h"
#include "texture.h"

using namespace std;

struct color : public glm::vec3 {
	color() : glm::vec3() {}
	color(const glm::vec3 &v) : glm::vec3(v) {}
	color(const float r, const float g, const float b) : glm::vec3(r, g, b) {}

	color(const int hex) {
		r = ((hex & 0x00ff0000) >> (8 * 2)) / 255.f;
		g = ((hex & 0x0000ff00) >> (8 * 1)) / 255.f;
		b = ((hex & 0x000000ff) >> (8 * 0)) / 255.f;
	}
};

struct material {
	color ambient = 0xffffff;
	color diffuse = 0xffffff;
	color specular = 0x4c4c4c;
	color emissive = 0x000000;

	float shininess = 25.f;

	shared_ptr<texture> tex = nullptr;
	shared_ptr<texture> normals = nullptr;
};

struct light {
    color ambient = 0x808080;
	color diffuse = 0xffffff;
	color specular = 0xffffff;

	glm::vec3 direction{ 0.f, 0.f, 1.f };
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
