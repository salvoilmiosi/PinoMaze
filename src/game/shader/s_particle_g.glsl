#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 50) out;

in float type0[];
in float age0[];
in vec3 position0[];
in vec3 velocity0[];
in vec4 data0[];

out float type1;
out float age1;
out vec3 position1;
out vec3 velocity1;
out vec4 data1;

uniform sampler2D randomTexture;
uniform float deltaMs;
uniform float globalTime;

const float gravityAccel = 3.f;
const float particleLifetime = 500.0;
const float sourceLifetime = 100.0;

#define TYPE_NONE           0.f
#define TYPE_SOURCE         1.f
#define TYPE_STATIC         2.f
#define TYPE_PARTICLE1      3.f
#define TYPE_PARTICLE2      4.f
#define TYPE_PARTICLE3      5.f
#define TYPE_PARTICLE4      6.f

vec3 randomVector(float seed) {
    float tex_coord = (globalTime + seed) / 1000.f;
    return texture(randomTexture, vec2(tex_coord, 0.0)).xyz;
}

void main() {
    float age = age0[0] + deltaMs;

    if (type0[0] == TYPE_SOURCE) {
        float num_particles = data0[0].x;
        float force = data0[0].y;
        float size = data0[0].z;
        float random_seed = data0[0].w;
        for (float i = 0; i < num_particles; ++i) {
            vec3 randVec = normalize(randomVector(random_seed + i) - vec3(0.5));
            type1 = floor(randomVector(random_seed + 2*i).x * 4.0) + TYPE_PARTICLE1;
            age1 = 0.0;
            position1 = position0[0] + randVec * size;
            velocity1 = velocity0[0] + randVec * force;
            data1.rgb = randomVector(random_seed + 3*i);
            data1.w = randomVector(random_seed + 4*i).x * 0.12 + 0.02;
            EmitVertex();
            EndPrimitive();
        }
    } else if (type0[0] == TYPE_STATIC) {
        float num_particles = data0[0].x;
        float force = data0[0].y;
        float size = data0[0].z;
        float random_seed = data0[0].w;
        if (age >= sourceLifetime) {
            for (float i = 0; i < num_particles; ++i) {
                vec3 randVec = normalize(randomVector(random_seed + i) - vec3(0.5));
                type1 = floor(randomVector(random_seed + 2*i).x * 4.0) + TYPE_PARTICLE1;
                age1 = 0.0;
                position1 = position0[0] + randVec * size;
                velocity1 = velocity0[0] + randVec * force;
                data1.rgb = randomVector(random_seed + 3*i);
                data1.w = randomVector(random_seed + 4*i).x * 0.12 + 0.02;
                EmitVertex();
                EndPrimitive();
            }
            age = 0.0;
        }
        type1 = TYPE_STATIC;
        age1 = age;
        position1 = position0[0];
        velocity1 = velocity0[0];
        data1 = data0[0];
        EmitVertex();
        EndPrimitive();
    } else if (type0[0] >= TYPE_PARTICLE1) {
        if (age < particleLifetime) {
            float deltaSecs = deltaMs / 1000.0;
            vec3 deltaP = deltaSecs * velocity0[0];
            vec3 deltaV = vec3(0.0, -gravityAccel, 0.0) * deltaSecs;

            type1 = type0[0];
            age1 = age;
            position1 = position0[0] + deltaP;
            velocity1 = velocity0[0] + deltaV;
            data1.rgb = data0[0].rgb;
            data1.w = data0[0].w * pow(0.99, deltaMs);
            EmitVertex();
            EndPrimitive();
        }
    }
}