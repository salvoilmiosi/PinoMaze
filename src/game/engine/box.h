#ifndef __BOX_H__
#define __BOX_H__

#include "model.h"

class box: public model {
public:
    box(float w = 1.f, float h = 1.f, float l = 1.f, float uv = 1.f);

public:
    const float width;
    const float height;
    const float length;
	const float texSize;
};

#endif // __BOX_H__
