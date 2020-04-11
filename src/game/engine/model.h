#ifndef __MODEL_H__
#define __MODEL_H__

#include "vertex_array.h"

#include <glm/glm.hpp>

struct vertex {
    glm::vec3 position;
    glm::vec2 texcoords;
};

struct normal_and_tangent {
    glm::vec3 normal;
    glm::vec3 tangent;
};

class model : public vertex_array {
public:
    model(draw_mode mode = DRAW_TRIANGLES) : vertex_array(mode) {}

    void calculate_buffers(const vertex *vertices, const size_t vertex_count, const GLuint *indices, const size_t index_count);
};

#endif // __MODEL_H__