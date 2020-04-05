#ifndef __SHADER_HUD_H__
#define __SHADER_HUD_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>

#include "../engine/shader_program.h"
#include "../engine/texture.h"

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

class hudRenderer {
protected:
    struct vertex {
        float x, y;
        float u, v;
    };

    std::vector<vertex> vertices;

    GLuint vertexBuffer = 0;
    GLuint vertexArray = 0;

    hudShader shader;

private:
	static const int STATUS_LENGTH = 128;
	static const int BUFFER_SIZE_DEFAULT = 128;

	std::string text;
	bool textChanged = false;

	texture TEX_FONT_TEXTURE;

public:
    virtual ~hudRenderer();

public:
    bool init();

	void setStatus(const char *str);
    void render();

private:
	int buildTextBuffer();

protected:
    glm::vec2 toWorldCoords(int x, int y);
    glm::vec2 toWorldVector(int x, int y);
};

#endif // __SHADER_HUD_H__
