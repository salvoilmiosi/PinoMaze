#ifndef __TRANSFORM_FEEDBACK_H__
#define __TRANSFORM_FEEDBACK_H__

#include "vertex_array.h"

class transform_feedback : public vertex_array {
public:
    transform_feedback(draw_mode mode = DRAW_POINTS);
    ~transform_feedback();

public:
    void init_buffer(const size_t size, std::initializer_list<vertex_attrib> attribs);

    void draw_feedback();

    void start();
    void stop();

private:
    GLuint gl_tfb;

    bool empty = true;
};

#endif // __TRANSFORM_FEEDBACK_H__