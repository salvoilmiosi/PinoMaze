#ifndef __HUD_SHADER_H__
#define __HUD_SHADER_H__

#include "shader_program.h"

class hudShader : public shaderProgram {
public:
    hudShader() : shaderProgram("hud") {}

public:
    bool init();

	bool bindProgram();
	void unbindProgram();

public:
    void setOffset(const glm::vec2 &v) {
        setUniform(offset, v);
    }
    void setDiffuseTexture(const unsigned int &i) {
        setUniform(diffuseTexture, i);
    }
    void setDiffuseColor(const glm::vec3 &v) {
        setUniform(diffuseColor, v);
    }

private:
    GLint offset;
    GLint diffuseTexture;
    GLint diffuseColor;

protected:
    void bindAddresses();
};

#endif // __HUD_SHADER_H__
