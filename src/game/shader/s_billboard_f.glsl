#version 330 core

uniform sampler2D particleTexture;

in vec2 texCoords;
in vec3 color;

out vec4 FragColor;

void main() {
    FragColor = texture2D(particleTexture, texCoords) * vec4(color, 1.0);
}