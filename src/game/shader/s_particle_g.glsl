#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 30) out;

in float type0[];
in float age0[];
in vec3 position0[];
in vec3 velocity0[];
in vec3 color0[];
in float size0[];

out float type1;
out float age1;
out vec3 position1;
out vec3 velocity1;
out vec3 color1;
out float size1;

uniform sampler2D randomTexture;
uniform float deltaMs;
uniform float globalTime;

const float gravityAccel = 3.0;
const float particleLifetime = 1000.0;
const float particleForce = 0.6;
const float distToSource = 0.07;

#define TYPE_NONE 0.f
#define TYPE_SOURCE 1.f
#define TYPE_PARTICLE1 2.f
#define TYPE_PARTICLE2 3.f
#define TYPE_PARTICLE3 4.f
#define TYPE_PARTICLE4 5.f

vec3 randomVector(float tex_coord) {
    return texture2D(randomTexture, vec2(tex_coord, 0.0)).xyz;
}

void main() {
    if (type0[0] == TYPE_SOURCE) {
        for (float i = 0; i < age0[0]; ++i) {
            vec3 randVec = normalize(randomVector((globalTime + i) / 1000.f) - vec3(0.5));
            type1 = floor(randomVector((globalTime + 3 * i) / 100.f).x * 4.0) + TYPE_PARTICLE1;
            age1 = 0.0;
            position1 = position0[0] + randVec * distToSource;
            velocity1 = velocity0[0] + randVec * particleForce;
            color1 = randomVector((globalTime + 2 * i) / 1000.f);
            size1 = 0.08;
            EmitVertex();
            EndPrimitive();
        }
    } else if (type0[0] >= TYPE_PARTICLE1) {
        float age = age0[0] + deltaMs;

        if (age < particleLifetime) {
            float deltaSecs = deltaMs / 1000.0;
            float t1 = age0[0] / 1000.0;
            float t2 = age / 1000.0;
            vec3 deltaP = deltaSecs * velocity0[0];
            vec3 deltaV = vec3(0.0, -gravityAccel, 0.0) * deltaSecs;

            type1 = type0[0];
            age1 = age;
            position1 = position0[0] + deltaP;
            velocity1 = velocity0[0] + deltaV;
            color1 = color0[0];
            size1 = size0[0] * 0.97;
            EmitVertex();
            EndPrimitive();
        }
    }
}