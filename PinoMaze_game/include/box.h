#ifndef __BOX_H__
#define __BOX_H__

#include "model.h"

class box: public model {
public:
    float width = 1.f;
    float height = 1.f;
    float length = 1.f;
	float texSize = 1.f;

public:
    void setSize(float w, float h, float l, float uv);

    bool init();
    void render();
};

#endif // __BOX_H__
