#ifndef __SHADER_SHADOW_H__
#define __SHADER_SHADOW_H__

#include "../engine/shader_program.h"

class shadowShader : public shaderProgram {
public:
    shadowShader() : shaderProgram("shadow") {}

public:
    bool init();
	
	bool bindProgram();
	void unbindProgram();

public:
    void setLightMatrix(const glm::mat4 &m) {
        setUniform(lightMatrix, m);
    }

private:
    GLint lightMatrix;

protected:
    void bindAddresses();
};

#endif // __SHADER_SHADOW_H__
