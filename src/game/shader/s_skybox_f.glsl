#version 330 core

in vec2 texCoords;
flat in float sampler_index;

out vec4 color;

uniform sampler2D skyboxTexture[6];

const float colors[6] = float[](1.0, 0.0, 0.0, 0.0, 0.0, 0.0);

void main() {
    //color = vec4(texCoords, colors[int(sampler_index)], 1.0);
    color = texture(skyboxTexture[int(sampler_index)], texCoords);
}
