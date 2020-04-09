#version 330 core

out vec4 FragColor;

in vec3 world_normal;

uniform vec3 light_direction;
uniform vec3 color;

void main() {
    float cos_theta = max(dot(world_normal, light_direction), 0.0);
    FragColor = vec4(color * (cos_theta * 0.9 + 0.1), 1.0);
}