#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resource.h"

renderer::renderer(context *con) : 
    program("renderer", SHADER_STRING(vertex), SHADER_STRING(fragment))
{
    program.add_uniform("transform_matrix", &transform_matrix);
    program.add_uniform("projection_matrix", &projection_matrix);
    program.add_uniform("view_matrix", &view_matrix);
    program.add_uniform("color", &color);
    program.add_uniform("light_direction", &light_direction);

    view_matrix = glm::lookAt(glm::vec3(0.f, 1.f, -2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.0));
    projection_matrix = glm::perspective(90.f, (float)con->window_width / (float)con->window_height, 0.5f, 5.f);
    light_direction = glm::normalize(light_direction);

    static const glm::mat4 matrices[] = {
        glm::translate(glm::mat4(1.f), glm::vec3(0.55f, 0.f, -0.55f)),
        glm::translate(glm::mat4(1.f), glm::vec3(-0.55f, 0.f, -0.55f)),
        glm::translate(glm::mat4(1.f), glm::vec3(0.55f, 0.f, 0.55f)),
        glm::translate(glm::mat4(1.f), glm::vec3(-0.55f, 0.f, 0.55f))
    };
    
    p_model.update_matrices(matrices, sizeof(matrices)/sizeof(glm::mat4), 5);
}

void renderer::tick() {
    color = glm::vec3((rand() % 255) / 255.f, (rand() % 255) / 255.f, (rand() % 255) / 255.f);
    static const glm::mat4 light_rotate_matrix = glm::rotate(glm::mat4(1.0), 0.05f, glm::vec3(-1.f, 0.f, 0.f));
    transform_matrix = glm::rotate(transform_matrix, 0.02f, glm::vec3(0.f, 1.f, 0.f));
    glm::vec4 light_4d = light_rotate_matrix * glm::vec4(light_direction, 1.f);
    light_direction.x = light_4d.x;
    light_direction.y = light_4d.y;
    light_direction.z = light_4d.z;
}

void renderer::render() {
    program.use_program();
    p_model.draw();
}