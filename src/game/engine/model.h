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

class model {
public:
    model();

    void draw() {
        vao.draw();
    }

    void update_matrices(const glm::mat4 *matrices, const size_t size, int location, bool dynamic = false) {
        vao.update_matrices(matrices, size, location, dynamic);
    }

protected:
    void calculate_buffers(const vertex *vertices, const size_t vertex_count, const GLuint *indices, const size_t index_count);

protected:
    vertex_array vao;
};

#endif // __BASE_MODEL_H__