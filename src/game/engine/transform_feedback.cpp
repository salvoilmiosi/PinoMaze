#include "transform_feedback.h"

#include "context.h"

transform_feedback::transform_feedback(draw_mode mode) : vertex_array(mode) {
    glGenTransformFeedbacks(1, &gl_tfb);
}

transform_feedback::~transform_feedback() {
    glDeleteTransformFeedbacks(1, &gl_tfb);
}

void transform_feedback::init_buffer(const size_t size, vertex_attrib_list attribs) {
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gl_tfb);
    alloc_buffer(0, size, attribs, true, 0);
    glBindBuffer(GL_ARRAY_BUFFER, gl_vbo[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, gl_vbo[0]);
}

void transform_feedback::draw_feedback() {
    if (empty) return;

    glBindVertexArray(gl_vao);
    glDrawTransformFeedback(gl_draw_mode, gl_tfb);
    glBindVertexArray(0);
}

void transform_feedback::start() {
    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gl_tfb);
    glBeginTransformFeedback(gl_draw_mode);
}

void transform_feedback::stop() {
    glDisable(GL_RASTERIZER_DISCARD);
    glEndTransformFeedback();
    empty = false;
}