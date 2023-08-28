#version 330 core

in vec2 texCoords;
flat in float sampler_index;

out vec4 color;

uniform sampler2D skyboxTexture[6];

void main() {
    switch (int(sampler_index)) {
    case 0:
        color = texture(skyboxTexture[0], texCoords); break;
    case 1:
        color = texture(skyboxTexture[1], texCoords); break;
    case 2:
        color = texture(skyboxTexture[2], texCoords); break;
    case 3:
        color = texture(skyboxTexture[3], texCoords); break;
    case 4:
        color = texture(skyboxTexture[4], texCoords); break;
    case 5:
        color = texture(skyboxTexture[5], texCoords); break;
    default:
        color = vec4(1.0);
    }
}
