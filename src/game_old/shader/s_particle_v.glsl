#version 330 core

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec3 position;
layout (location = 2) in vec3 partColor;
layout (location = 3) in vec2 uvOffset;
layout (location = 4) in float particleSize;

out vec2 texCoords;
out vec4 particleColor;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float texSize;

void main() {
	vec3 cameraRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
	vec3 cameraUp = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

	vec3 positionWorld = position + cameraRight * vertex.x * particleSize + cameraUp * vertex.y * particleSize;

    gl_Position = projectionMatrix * viewMatrix * vec4(positionWorld, 1.0);

	vec2 uv = vertex * 0.5 + 0.5;
    texCoords = uvOffset + uv * texSize;

	particleColor = vec4(partColor, 1.0);
}
