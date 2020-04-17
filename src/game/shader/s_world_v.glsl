#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in mat4 modelMatrix;
layout (location = 8) in vec2 tpUvOffset;
layout (location = 9) in float wallValue;

out vec2 texCoords;
out vec2 tpTileCoords;
out vec3 worldNormal;
out vec3 toCamera;
out vec4 shadowCoords;

out vec3 tangentLight;
out vec3 tangentCamera;
out vec4 wallColor;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 lightMatrix;

uniform float refractionHeight;

struct light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

const vec4[] wallColors = vec4[](
    vec4(1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(1.0, 0.0, 1.0, 1.0)
);

uniform light sun;

void main() {
    texCoords = uv;
    tpTileCoords = tpUvOffset;
    wallColor = wallColors[int(wallValue)];

    mat4 modelViewMatrix = viewMatrix * modelMatrix;

    vec4 vectorPosition = modelMatrix * vec4(position, 1.0);
    vec4 cameraPosition = inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0);
    vec4 cameraVec = vectorPosition - cameraPosition;

    gl_Position = projectionMatrix * viewMatrix * vectorPosition;
	
	vec4 plane = vec4(0.0, -1.0, 0.0, refractionHeight);
	gl_ClipDistance[0] = dot(vectorPosition, plane);

    vec3 norm = normalize((modelViewMatrix * vec4(normal, 0.0)).xyz);
    vec3 tang = normalize((modelViewMatrix * vec4(tangent, 0.0)).xyz);
    vec3 bitang = normalize(cross(norm, tang));

    mat3 tangentMatrix = mat3(
        tang.x, bitang.x, norm.x,
        tang.y, bitang.y, norm.y,
        tang.z, bitang.z, norm.z
    );

    toCamera = (viewMatrix * cameraVec).xyz;
    shadowCoords = lightMatrix * vectorPosition;
    worldNormal = norm;
    tangentLight = (tangentMatrix * sun.direction).xyz;
    tangentCamera = tangentMatrix * toCamera;
}
