#version 330 core

layout (location = 0) in float type;
layout (location = 2) in vec3 position;
layout (location = 4) in vec4 data;

out float type0;
out vec3 position0;
out vec4 data0;

void main() {
    type0 = type;
    position0 = position;
    data0 = data;
}