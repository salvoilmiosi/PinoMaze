#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in int type0[];
in float age0[];
in vec3 position0[];
in vec3 velocity0[];
in vec3 color0[];
in float size0[];

out int type1;
out float age1;
out vec3 position1;
out vec3 velocity1;
out vec3 color1;
out float size1;

uniform sampler2D randomTexture;
uniform float deltaMs;
uniform float globalTime;
uniform float gravityAccel;
uniform float particleLifetime;

#define TYPE_NONE 0
#define TYPE_SOURCE 1
#define TYPE_PARTICLE 2

vec3 getRandomVector(float tex_coord) {
    vec3 dir = texture2D(randomTexture, vec2(tex_coord, 0.0)).xyz;
    dir -= vec3(0.5, 0.5, 0.5);
    return dir;
}

vec3 getRandomColor(float tex_coord) {
    return texture2D(randomTexture, vec2(tex_coord, 0.0)).xyz;
}

void main() {
    float age = age0[0] + deltaMs;

    if (type0[0] == TYPE_SOURCE) {
        for (int i = 0; i < 20; ++i) {
            type1 = TYPE_PARTICLE;
            age1 = 0.0;
            position1 = position0[0];
            velocity1 = normalize(getRandomVector((globalTime + i) / 1000.f)) * 0.05;
            color1 = getRandomColor((globalTime + 2 * i) / 1000.f);
            size1 = 0.1;
            EmitVertex();
            EndPrimitive();
        }
    } else if (type0[0] == TYPE_PARTICLE) {
        if (age < particleLifetime) {
            float deltaSecs = deltaMs / 1000.0;
            float t1 = age0[0] / 1000.0;
            float t2 = age / 1000.0;
            vec3 deltaP = deltaSecs * velocity0[0];
            vec3 deltaV = vec3(0.0, -gravityAccel, 0.0) * deltaSecs;

            type1 = TYPE_PARTICLE;
            age1 = age;
            position1 = position0[0] + deltaP;
            velocity1 = velocity0[0] + deltaV;
            color1 = color0[0];
            size1 = size0[0];
            EmitVertex();
            EndPrimitive();
        }
    }
}