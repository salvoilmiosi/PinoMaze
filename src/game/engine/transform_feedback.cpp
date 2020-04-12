#include "transform_feedback.h"

#include "context.h"

transform_feedback::transform_feedback(draw_mode mode) : vertex_array(mode) {
    glGenTransformFeedbacks(1, &gl_tfb);
}

transform_feedback::~transform_feedback() {
    glDeleteTransformFeedbacks(1, &gl_tfb);
}

void transform_feedback::update_buffer(const void *data, const size_t size, std::initializer_list<vertex_attrib> attribs) {
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gl_tfb);
    update_vertices(0, data, size, attribs, true);
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, gl_vbo[0]);
}

void transform_feedback::draw_feedback() {
    glBindVertexArray(gl_vao);
    glDrawTransformFeedback(gl_draw_mode, gl_tfb);
    glBindVertexArray(0);
}

void transform_feedback::start() {
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gl_tfb);
    glBeginTransformFeedback(gl_draw_mode);
    checkGlError("a");
}

void transform_feedback::stop() {
    glEndTransformFeedback();
}