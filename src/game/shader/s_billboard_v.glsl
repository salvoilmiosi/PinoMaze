#version 330 core

layout (location = 0) in float type;
layout (location = 2) in vec3 position;
layout (location = 4) in vec3 color;
layout (location = 5) in float size;

out float type0;
out vec3 position0;
out vec3 color0;
out float size0;

void main() {
    type0 = type;
    position0 = position;
    color0 = color;
    size0 = size;
}