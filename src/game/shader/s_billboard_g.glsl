#version 330 core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

in vec3 position0[];
in vec3 color0[];
in float size0[];

out vec2 texCoords;
out vec3 color;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;

void main() {
    mat4 viewProj = projMatrix * viewMatrix;
    
    vec3 pos = position0[0];
    vec3 toCamera = normalize(cameraPos - pos).xyz;
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(toCamera, up) * size0[0];

    // bottom left
    pos -= right * 0.5;
    pos.y -= size0[0] * 0.5;
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.5, 1.0);
    color = color0[0];
    EmitVertex();

    // top left
    pos.y += size0[0];
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(0.5, 0.5);
    color = color0[0];
    EmitVertex();

    // bottom right
    pos += right;
    pos.y -= size0[0];
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(1.0, 1.0);
    color = color0[0];
    EmitVertex();

    // top right
    pos.y += size0[0];
    gl_Position = viewProj * vec4(pos, 1.0);
    texCoords = vec2(1.0, 0.5);
    color = color0[0];
    EmitVertex();

    EndPrimitive();
}