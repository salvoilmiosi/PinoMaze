#version 330 core

in vec2 texCoords;

out vec4 color;

uniform sampler2D diffuseTexture;
uniform vec3 diffuseColor;

void main() {
    color = texture(diffuseTexture, texCoords) * vec4(diffuseColor, 1.0);
}
