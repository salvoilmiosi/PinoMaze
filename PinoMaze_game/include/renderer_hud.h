#ifndef __HUD_H__
#define __HUD_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>

using namespace std;

#include "texture.h"

#include "shader_hud.h"

class hudRenderer {
protected:
    struct vertex {
        float x, y;
        float u, v;
    };

    vector<vertex> vertices;

    GLuint vertexBuffer = 0;
    GLuint vertexArray = 0;

    hudShader shader;

private:
	static const int STATUS_LENGTH = 128;
	static const int BUFFER_SIZE_DEFAULT = 128;

	string text;
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

#endif // __HUD_H__
