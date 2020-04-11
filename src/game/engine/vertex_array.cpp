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
    {2, GL_FLOAT, 2 * sizeof(float)},
    {3, GL_FLOAT, 3 * sizeof(float)},
    {4, GL_FLOAT, 4 * sizeof(float)}
};

static constexpr GLenum gl_draw_modes[] = {
    GL_POINTS,
    GL_LINES,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN
};

vertex_array::vertex_array(draw_mode mode, size_t num_vbos) :
    num_vbos(num_vbos),
    gl_draw_mode(gl_draw_modes[mode])
{
    glGenVertexArrays(1, &gl_vao);
    glGenBuffers(num_vbos, gl_vbo);
    glGenBuffers(1, &gl_mat_vbo);
    glGenBuffers(1, &gl_ebo);
}

vertex_array::~vertex_array() {
    glDeleteVertexArrays(1, &gl_vao);
    glDeleteBuffers(num_vbos, gl_vbo);
    glDeleteBuffers(1, &gl_mat_vbo);
    glDeleteBuffers(1, &gl_ebo);
}

void vertex_array::update_buffer(size_t vbo_index, const void *data, const size_t size, std::initializer_list<vertex_attrib> attribs, bool dynamic) {
    glBindVertexArray(gl_vao);
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[vbo_index]);
    glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    GLsizei stride = 0;
    for (auto &a : attribs) {
        stride += attrib_gl_info[a.type].size;
    }

    size_t start = 0;
    for (auto &a : attribs) {
        glVertexAttribPointer(a.location, attrib_gl_info[a.type].num, attrib_gl_info[a.type].type, GL_FALSE, stride, (void *)start);
        start += attrib_gl_info[a.type].size;
        glEnableVertexAttribArray(a.location);
    }

    glBindVertexArray(0);
}

void vertex_array::update_indices(const unsigned int *data, const size_t size, bool dynamic) {
    glBindVertexArray(gl_vao);
    index_count = size;
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void vertex_array::update_matrices(const glm::mat4 *data, const size_t size, int location, bool dynamic) {
    if (size == 0) return;
    
    glBindVertexArray(gl_vao);
    matrix_count = size;

    glBindBuffer(GL_ARRAY_BUFFER, gl_mat_vbo);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::mat4), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glEnableVertexAttribArray(location);
    glEnableVertexAttribArray(location + 1);
    glEnableVertexAttribArray(location + 2);
    glEnableVertexAttribArray(location + 3);

    glVertexAttribDivisor(location,     1);
    glVertexAttribDivisor(location + 1, 1);
    glVertexAttribDivisor(location + 2, 1);
    glVertexAttribDivisor(location + 3, 1);

    glVertexAttribPointer(location,     4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)0);
    glVertexAttribPointer(location + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(    sizeof(glm::vec4)));
    glVertexAttribPointer(location + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
    glVertexAttribPointer(location + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));

    glBindVertexArray(0);
}

void vertex_array::draw() {
    if (index_count > 0) {
        if (matrix_count > 0) {
            glBindVertexArray(gl_vao);
            glDrawElementsInstanced(gl_draw_mode, index_count, GL_UNSIGNED_INT, (void *)0, matrix_count);
            glBindVertexArray(0);
        } else {
            glBindVertexArray(gl_vao);
            glDrawElements(gl_draw_mode, index_count, GL_UNSIGNED_INT, (void *)0);
            glBindVertexArray(0);
        }
    }
}

void vertex_array::draw(size_t count, size_t offset) {
    if (index_count > 0) {
        if (count == 0) {
            count = index_count - offset;
        }
        if (matrix_count > 0) {
            glBindVertexArray(gl_vao);
            glDrawElementsInstanced(gl_draw_mode, count, GL_UNSIGNED_INT, (void *)(offset * sizeof(GLuint)), matrix_count);
            glBindVertexArray(0);
        } else {
            glBindVertexArray(gl_vao);
            glDrawElements(gl_draw_mode, count, GL_UNSIGNED_INT, (void *)(offset * sizeof(GLuint)));
            glBindVertexArray(0);
        }
    }
}