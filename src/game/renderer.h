#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "engine/entity.h"
#include "engine/shader.h"
#include "engine/box.h"
#include "engine/sphere.h"
#include "engine/context.h"

#include <glm/glm.hpp>

class renderer : public entity {
public:
    renderer(context *con);

public:
    void tick();
    void render();

private:
    glm::mat4 transform_matrix = glm::mat4(1.f);
    glm::mat4 view_matrix = glm::mat4(1.f);
    glm::mat4 projection_matrix = glm::mat4(1.f);

    glm::vec3 color{0.f, 1.f, 0.f};
    glm::vec3 light_direction{-0.5f, 0.5f, 0.5f};

    shader program;
    sphere p_model;
};

#endif // __RENDERER_H__