#ifndef __TRUNC_CONE_H__
#define __TRUNC_CONE_H__

#include "base_model.h"

class trunc_cone : public base_model {
public:
    trunc_cone(float r1, float r2, float h, int divs);

public:
    const float r1;
    const float r2;
    const float h;
    const float divs;
};

#endif