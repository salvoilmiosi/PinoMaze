#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in mat4 modelMatrix;

out vec2 texCoords;
out vec4 clipSpace;
out vec3 toCamera;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 cameraPosition;

void main() {
    vec4 vectorPosition = modelMatrix * vec4(position, 1.0);
    vec4 cameraVec = vectorPosition - vec4(cameraPosition, 1.0);

    clipSpace = projectionMatrix * viewMatrix * vectorPosition;
    gl_Position = clipSpace;
	
    texCoords = (modelMatrix * vec4(position, 1.0)).xz;
    toCamera = cameraVec.xyz;
}

