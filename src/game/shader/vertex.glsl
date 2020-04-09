#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 5) in mat4 model_matrix;

out vec3 world_normal;

uniform mat4 transform_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main() {
    mat4 model_view = view_matrix * transform_matrix * model_matrix;
    gl_Position = projection_matrix * model_view * vec4(position, 1.0);

    world_normal = normalize((model_view * vec4(normal, 0.0)).xyz);
}