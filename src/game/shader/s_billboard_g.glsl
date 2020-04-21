#version 330 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

in float type0[];
in vec3 position0[];
in vec4 data0[]; // (color rgb, size)

out vec2 texCoords;
out vec3 color;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;

#define TYPE_NONE           0.f
#define TYPE_SOURCE         1.f
#define TYPE_STATIC         2.f
#define TYPE_PARTICLE1      3.f
#define TYPE_PARTICLE2      4.f
#define TYPE_PARTICLE3      5.f
#define TYPE_PARTICLE4      6.f

void main() {
    mat4 viewProj = projMatrix * viewMatrix;
    
    vec3 pos = position0[0];
    vec3 toCamera = normalize(cameraPos - pos).xyz;
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up) * data0[0].w;
    up = up * data0[0].w;

    vec2 uvOffset = vec2(0.0);
    if (type0[0] == TYPE_PARTICLE1) {
        uvOffset = vec2(0.0, 0.0);
    } else if (type0[0] == TYPE_PARTICLE2) {
        uvOffset = vec2(0.5, 0.0);
    } else if (type0[0] == TYPE_PARTICLE3) {
        uvOffset = vec2(0.0, 0.5);
    } else if (type0[0] == TYPE_PARTICLE4) {
        uvOffset = vec2(0.5, 0.5);
    } else {
        return;
    }

    // bottom left
    pos -= (right + up) * 0.5;
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.0, 0.5) + uvOffset;
    color = data0[0].rgb;
    EmitVertex();

    // top left
    pos += up;
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.0, 0.0) + uvOffset;
    color = data0[0].rgb;
    EmitVertex();

    // bottom right
    pos += right - up;
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.5, 0.5) + uvOffset;
    color = data0[0].rgb;
    EmitVertex();

    // top right
    pos += up;
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.5, 0.0) + uvOffset;
    color = data0[0].rgb;
    EmitVertex();

    EndPrimitive();
}