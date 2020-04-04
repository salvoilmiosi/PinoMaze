#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "model.h"
#include "globals.h"

#include <vector>



class sphere : public model {
private:
    float sphereRadius = 0.5f;
    int sphereSubsH = 16;
    int sphereSubsV = 8;

    std::vector<vertex> vertices;
    std::vector<GLuint> indices;

public:
    void setSize(float r, int sh, int sv);

    bool init();

    void render();
};

#endif // __SPHERE_H__
