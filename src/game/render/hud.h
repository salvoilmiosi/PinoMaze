#ifndef __SHADER_HUD_H__
#define __SHADER_HUD_H__

#include <GL/glew.h>
#include <GL/gl.h>

#include "../engine/engine.h"
#include "../engine/shader.h"
#include "../engine/texture.h"
#include "../engine/vertex_array.h"

class hud {
public:
    hud(engine *m_engine);

public:
    void render(float deltaNano);

private:
    const engine_options &options;

	static const int STATUS_LENGTH = 128;
	static const int BUFFER_SIZE_DEFAULT = 128;

    vf_shader m_shader;
    vertex_array vao;

    int diffuseSampler = 0;
    glm::vec2 m_offset;
    glm::vec3 m_color;
    
    glm::vec2 toWorldCoords(int x, int y);
    glm::vec2 toWorldVector(int x, int y);
};

#endif // __SHADER_HUD_H__
