#ifndef __TRANSFORM_FEEDBACK_H__
#define __TRANSFORM_FEEDBACK_H__

#include "vertex_array.h"

class transform_feedback : public vertex_array {
public:
    transform_feedback(draw_mode mode = DRAW_POINTS);
    ~transform_feedback();

public:
    void update_buffer(const void *data, const size_t size, std::initializer_list<vertex_attrib> attribs);

    void draw_feedback();

    void start();
    static void stop();

private:
    GLuint gl_tfb;
};

#endif // __TRANSFORM_FEEDBACK_H__