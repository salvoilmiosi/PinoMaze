#include "vertex_array.h"

static constexpr struct {
    GLint num;
    GLenum type;
    GLenum size;
} attrib_gl_info[] = {
    {1, GL_BYTE, sizeof(char)},
    {1, GL_UNSIGNED_BYTE, sizeof(char)},
    {1, GL_SHORT, sizeof(short)},
    {1, GL_UNSIGNED_SHORT, sizeof(short)},
    {1, GL_INT, sizeof(int)},
    {1, GL_UNSIGNED_INT, sizeof(int)},
    {1, GL_FLOAT, sizeof(float)},
    {1, GL_DOUBLE, sizeof(double)},
    {2, GL_FLOAT, sizeof(glm::vec2)},
    {3, GL_FLOAT, sizeof(glm::vec3)},
    {4, GL_FLOAT, sizeof(glm::vec4)},
    {16, GL_FLOAT, sizeof(glm::mat4)}
};

static constexpr GLenum gl_draw_modes[] = {
    GL_POINTS,
    GL_LINES,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN
};

vertex_array::vertex_array(draw_mode mode) :
    gl_draw_mode(gl_draw_modes[mode])
{
    glGenVertexArrays(1, &gl_vao);
}

vertex_array::~vertex_array() {
    glDeleteVertexArrays(1, &gl_vao);
    glDeleteBuffers(num_vbos, gl_vbo);
    if (gl_ebo) glDeleteBuffers(1, &gl_ebo);
}

static constexpr size_t get_stride(vertex_attrib_list attribs) {
    size_t ret = 0;
    for (auto &a : attribs) {
        ret += attrib_gl_info[a.type].size;
    }
    return ret;
}

void vertex_array::alloc_buffer(size_t vbo_index, const size_t size, vertex_attrib_list attribs, bool dynamic, size_t divisor) {
    buffer_size[vbo_index] = size;
    if (buffer_size[vbo_index] == 0) return;
    
    if (vbo_index >= num_vbos) {
        glGenBuffers(1, gl_vbo + vbo_index);
        num_vbos = vbo_index + 1;
    }

    glBindVertexArray(gl_vao);
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);

    size_t stride = get_stride(attribs);
    
    size_t start = 0;
    for (auto &a : attribs) {
        if (a.type == ATTR_MAT4) {
            glEnableVertexAttribArray(a.location);
            glEnableVertexAttribArray(a.location + 1);
            glEnableVertexAttribArray(a.location + 2);
            glEnableVertexAttribArray(a.location + 3);

            glVertexAttribDivisor(a.location,     divisor);
            glVertexAttribDivisor(a.location + 1, divisor);
            glVertexAttribDivisor(a.location + 2, divisor);
            glVertexAttribDivisor(a.location + 3, divisor);

            glVertexAttribPointer(a.location,     4, GL_FLOAT, GL_FALSE, stride, (void *)(start));
            glVertexAttribPointer(a.location + 1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start +     sizeof(glm::vec4)));
            glVertexAttribPointer(a.location + 2, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start + 2 * sizeof(glm::vec4)));
            glVertexAttribPointer(a.location + 3, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start + 3 * sizeof(glm::vec4)));
        } else {
            glEnableVertexAttribArray(a.location);
            glVertexAttribDivisor(a.location, divisor);
            glVertexAttribPointer(a.location, attrib_gl_info[a.type].num, attrib_gl_info[a.type].type, GL_FALSE, stride, (void *)start);
        }
        
        start += attrib_gl_info[a.type].size;
    }

    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);
    glBufferData(GL_ARRAY_BUFFER, size * stride, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void vertex_array::update_vertices(size_t vbo_index, const void *data, const size_t size, vertex_attrib_list attribs, bool dynamic) {
    num_vertices = size;
    if (num_vertices == 0) return;

    if (num_vertices > buffer_size[vbo_index]) {
        alloc_buffer(vbo_index, size, attribs, dynamic, 0);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * get_stride(attribs), data);
}

void vertex_array::update_instances(size_t vbo_index, const void *data, const size_t size, vertex_attrib_list attribs, bool dynamic) {
    num_instances = size;
    if (num_instances == 0) return;

    if (num_instances > buffer_size[vbo_index]) {
        alloc_buffer(vbo_index, size, attribs, dynamic, 1);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * get_stride(attribs), data);
}

void vertex_array::update_indices(const unsigned int *data, const size_t size, bool dynamic) {
    size_t old_size = num_indices;
    num_indices = size;

    if (num_indices == 0) return;

    if (!gl_ebo) {
        glGenBuffers(1, &gl_ebo);
    }

    glBindVertexArray(gl_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_ebo);
    if (num_indices > old_size) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    } else {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size * sizeof(unsigned int), data);
    }
    glBindVertexArray(0);
}

void vertex_array::draw(size_t first, size_t count) {
    glBindVertexArray(gl_vao);
    if (num_indices > 0) {
        if (count == 0) {
            count = num_indices - first;
        }
        glDrawElements(gl_draw_mode, count, GL_UNSIGNED_INT, (void *)(first * sizeof(GLuint)));
    } else if (num_vertices > 0) {
        if (count == 0) {
            count = num_vertices - first;
        }
        glDrawArrays(gl_draw_mode, first, count);
    }
    glBindVertexArray(0);
}

void vertex_array::draw_instances(size_t first, size_t count) {
    if (num_instances == 0) {
        return;
    }
    glBindVertexArray(gl_vao);
    if (num_indices > 0) {
        if (count == 0) {
            count = num_indices - first;
        }
        glDrawElementsInstanced(gl_draw_mode, count, GL_UNSIGNED_INT, (void *)(first * sizeof(GLuint)), num_instances);
    } else if (num_vertices > 0) {
        if (count == 0) {
            count = num_vertices - first;
        }
        glDrawArraysInstanced(gl_draw_mode, first, count, num_instances);
    }
    glBindVertexArray(0);
}