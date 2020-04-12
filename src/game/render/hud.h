#ifndef __SHADER_HUD_H__
#define __SHADER_HUD_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <string>

#include "../engine/entity.h"
#include "../engine/shader.h"
#include "../engine/texture.h"
#include "../engine/vertex_array.h"

class hud : public entity {
public:
    hud(context *m_context);

public:
    void render();
	void setStatus(const std::string &status);

private:
	static const int STATUS_LENGTH = 128;
	static const int BUFFER_SIZE_DEFAULT = 128;

	std::string text;
	bool textChanged = false;

    vf_shader m_shader;
    vertex_array vao;

    sampler m_sampler{0};
    glm::vec2 m_offset;
    glm::vec3 m_color;

private:
    glm::vec2 toWorldCoords(int x, int y);
    glm::vec2 toWorldVector(int x, int y);
};

#endif // __SHADER_HUD_H__
