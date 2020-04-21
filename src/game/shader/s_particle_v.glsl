#version 330 core

layout (location = 0) in float type;
layout (location = 1) in float age;
layout (location = 2) in vec3 position;
layout (location = 3) in vec3 velocity;
layout (location = 4) in vec4 data;

out float type0;
out float age0;
out vec3 position0;
out vec3 velocity0;
out vec4 data0;

void main() {
	type0 = type;
	age0 = age;
	position0 = position;
	velocity0 = velocity;
	data0 = data;
}