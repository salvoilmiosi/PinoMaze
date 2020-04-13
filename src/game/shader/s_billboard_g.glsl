#version 330 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

in float type0[];
in vec3 position0[];
in vec3 color0[];
in float size0[];

out vec2 texCoords;
out vec3 color;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;

#define TYPE_NONE 0.f
#define TYPE_SOURCE 1.f
#define TYPE_PARTICLE1 2.f
#define TYPE_PARTICLE2 3.f
#define TYPE_PARTICLE3 4.f
#define TYPE_PARTICLE4 5.f

void main() {
    mat4 viewProj = projMatrix * viewMatrix;
    
    vec3 pos = position0[0];
    vec3 toCamera = normalize(cameraPos - pos).xyz;
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up) * size0[0];

    vec2 uvOffset = vec2(0.0);
    if (type0[0] == TYPE_PARTICLE1) {
        uvOffset = vec2(0.0, 0.0);
    } else if (type0[0] == TYPE_PARTICLE2) {
        uvOffset = vec2(0.5, 0.0);
    } else if (type0[0] == TYPE_PARTICLE3) {
        uvOffset = vec2(0.0, 0.5);
    } else if (type0[0] == TYPE_PARTICLE4) {
        uvOffset = vec2(0.5, 0.5);
    }

    // bottom left
    pos -= right * 0.5;
    pos.y -= size0[0] * 0.5;
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.0, 0.5) + uvOffset;
    color = color0[0];
    EmitVertex();

    // top left
    pos.y += size0[0];
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.0, 0.0) + uvOffset;
    color = color0[0];
    EmitVertex();

    // bottom right
    pos += right;
    pos.y -= size0[0];
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.5, 0.5) + uvOffset;
    color = color0[0];
    EmitVertex();

    // top right
    pos.y += size0[0];
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.5, 0.0) + uvOffset;
    color = color0[0];
    EmitVertex();

    EndPrimitive();
}