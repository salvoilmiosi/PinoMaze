#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "model.h"

#include <vector>

class sphere : public model {
public:
    sphere(float r = 0.5f, int sh = 16, int sv = 8);

private:
    const float sphereRadius;
    const int sphereSubsH;
    const int sphereSubsV;
};

#endif // __SPHERE_H__
