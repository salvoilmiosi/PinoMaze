#ifndef __BOX_H__
#define __BOX_H__

#include "base_model.h"

class box: public base_model {
public:
    box(float w = 1.f, float h = 1.f, float l = 1.f, float uv = 1.f);

public:
    const float width;
    const float height;
    const float length;
	const float texSize;
};

#endif // __BOX_H__
