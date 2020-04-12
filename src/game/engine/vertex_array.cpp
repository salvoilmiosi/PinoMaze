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

void vertex_array::update_vertices(size_t vbo_index, const void *data, const size_t size, std::initializer_list<vertex_attrib> attribs, bool dynamic) {
    if (size == 0) return;
    
    GLsizei stride = 0;
    for (auto &a : attribs) {
        stride += attrib_gl_info[a.type].size;
    }
    
    bool created = false;
    size_t old_size = num_vertices;
    num_vertices = size / stride;
    
    glBindVertexArray(gl_vao);

    if (vbo_index >= num_vbos) {
        created = true;
        num_vbos = vbo_index+1;

        glGenBuffers(1, gl_vbo + vbo_index);
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);
        
        size_t start = 0;
        for (auto &a : attribs) {
            if (a.type == ATTR_MAT4) {
                glEnableVertexAttribArray(a.location);
                glEnableVertexAttribArray(a.location + 1);
                glEnableVertexAttribArray(a.location + 2);
                glEnableVertexAttribArray(a.location + 3);

                glVertexAttribPointer(a.location,     4, GL_FLOAT, GL_FALSE, stride, (void *)(start));
                glVertexAttribPointer(a.location + 1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start +     sizeof(glm::vec4)));
                glVertexAttribPointer(a.location + 2, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start + 2 * sizeof(glm::vec4)));
                glVertexAttribPointer(a.location + 3, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start + 3 * sizeof(glm::vec4)));
            } else {
                glEnableVertexAttribArray(a.location);
                glVertexAttribPointer(a.location, attrib_gl_info[a.type].num, attrib_gl_info[a.type].type, GL_FALSE, stride, (void *)start);
            }
            
            start += attrib_gl_info[a.type].size;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);
    if (created || num_vertices > old_size) {
        glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
    glBindVertexArray(0);
}

void vertex_array::update_instances(size_t vbo_index, const void *data, const size_t size, std::initializer_list<vertex_attrib> attribs, bool dynamic) {
    if (size == 0) return;
    
    GLsizei stride = 0;
    for (auto &a : attribs) {
        stride += attrib_gl_info[a.type].size;
    }

    size_t old_size = num_instances;
    bool created = false;
    num_instances = size / stride;

    glBindVertexArray(gl_vao);

    if (vbo_index >= num_vbos) {
        created = true;
        num_vbos = vbo_index+1;

        glGenBuffers(1, gl_vbo + vbo_index);
        glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);

        size_t start = 0;
        for (auto &a : attribs) {
            if (a.type == ATTR_MAT4) {
                glEnableVertexAttribArray(a.location);
                glEnableVertexAttribArray(a.location + 1);
                glEnableVertexAttribArray(a.location + 2);
                glEnableVertexAttribArray(a.location + 3);

                glVertexAttribDivisor(a.location,     1);
                glVertexAttribDivisor(a.location + 1, 1);
                glVertexAttribDivisor(a.location + 2, 1);
                glVertexAttribDivisor(a.location + 3, 1);

                glVertexAttribPointer(a.location,     4, GL_FLOAT, GL_FALSE, stride, (void *)(start));
                glVertexAttribPointer(a.location + 1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start +     sizeof(glm::vec4)));
                glVertexAttribPointer(a.location + 2, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start + 2 * sizeof(glm::vec4)));
                glVertexAttribPointer(a.location + 3, 4, GL_FLOAT, GL_FALSE, stride, (void *)(start + 3 * sizeof(glm::vec4)));
            } else {
                glEnableVertexAttribArray(a.location);
                glVertexAttribDivisor(a.location, 1);
                glVertexAttribPointer(a.location, attrib_gl_info[a.type].num, attrib_gl_info[a.type].type, GL_FALSE, stride, (void *)start);
            }
            
            start += attrib_gl_info[a.type].size;
        }

    }
        
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);
    if (created || num_instances > old_size) {
        glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
    glBindVertexArray(0);
}

void vertex_array::update_indices(const unsigned int *data, const size_t size, bool dynamic) {
    if (size == 0) return;
    
    size_t old_size = num_indices;
    num_indices = size;


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