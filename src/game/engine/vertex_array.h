#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include <initializer_list>

enum attrib_type {
    ATTR_BYTE,
    ATTR_UNSIGNED_BYTE,
    ATTR_SHORT,
    ATTR_UNSIGNED_SHORT,
    ATTR_INT,
    ATTR_UNSIGNED_INT,
    ATTR_FLOAT,
    ATTR_DOUBLE,
    ATTR_VEC2,
    ATTR_VEC3,
    ATTR_VEC4,
    ATTR_MAT4
};

enum draw_mode {
    DRAW_POINTS,
    DRAW_LINES,
    DRAW_TRIANGLES,
    DRAW_TRIANGLE_STRIP,
    DRAW_TRIANGLE_FAN
};

struct vertex_attrib {
    int location;
    attrib_type type;
};

constexpr size_t MAX_VBOS = 5;

class vertex_array {
public:
    vertex_array(draw_mode mode = DRAW_TRIANGLES);
    ~vertex_array();

    void update_vertices(size_t vbo_index, const void *data, const size_t size, std::initializer_list<vertex_attrib> attribs, bool dynamic = false);
    void update_instances(size_t vbo_index, const void *data, const size_t size, std::initializer_list<vertex_attrib> attribs, bool dynamic = false);
    void update_indices(const unsigned int *data, const size_t size, bool dynamic = false);
    
    void update_matrices(size_t vbo_index, const glm::mat4 *matrices, const size_t size, int location, bool dynamic = false) {
        update_instances(vbo_index, matrices, size * sizeof(glm::mat4), {{location, ATTR_MAT4}}, dynamic);
    }

    void draw(size_t first = 0, size_t count = 0);
    void draw_instances(size_t first = 0, size_t count = 0);

protected:
    size_t num_vbos = 0;
    
    GLuint gl_vao = 0;
    GLuint gl_vbo[MAX_VBOS];
    GLuint gl_ebo = 0;

    GLenum gl_draw_mode;

    size_t num_vertices = 0;
    size_t num_indices = 0;
    size_t num_instances = 0;
};

#endif // __VERTEX_ARRAY_H__