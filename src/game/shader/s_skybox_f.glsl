#version 330 core

in vec2 texCoords;
flat in float sampler_index;

out vec4 color;

uniform sampler2D skyboxTexture[6];

void main() {
    color = texture(skyboxTexture[int(sampler_index)], texCoords);
}
