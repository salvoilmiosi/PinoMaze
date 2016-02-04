#version 330 core

layout (location = 0) in vec3 position;
layout (location = 4) in mat4 modelMatrix;

uniform mat4 lightMatrix;

void main() {
    gl_Position = lightMatrix * modelMatrix * vec4(position, 1.0);
}
