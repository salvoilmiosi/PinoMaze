#version 330 core

in vec2 texCoords;
in vec2 tpTileCoords;
in vec3 worldNormal;
in vec3 toCamera;
in vec4 shadowCoords;

in vec3 tangentLight;
in vec3 tangentCamera;

out vec4 color;

uniform mat4 lightMatrix;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform sampler2D tpTileTexture;

#define ENABLE_TEXTURE      (1 << 0)
#define ENABLE_NORMALS      (1 << 1)
#define ENABLE_SPECULAR     (1 << 2)
#define ENABLE_SHADOWS      (1 << 3)
#define ENABLE_TELEPORT     (1 << 4)

uniform int renderFlags;

const vec4 tileDiffuse = vec4(0.21, 0.67, 1.0, 0.8);

struct light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

struct material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

uniform light sun;
uniform material mat;

uniform float shadowBias = 0.0005;
uniform vec2 shadowTexelSize = vec2(1.0 / 1024.0);

float sampleShadow(sampler2D tex, vec2 uv, float compare) {
    return step(compare, texture(tex, uv).z + shadowBias);
}

float shadowLinear(sampler2D tex, vec2 uv, float compare) {
    if (uv.x < 0.0 || uv.y < 0.0 || uv.x > 1.0 || uv.y > 1.0 || compare > 1.0) {
        return 1.0;
    }
    vec2 pixelPos = uv / shadowTexelSize + vec2(0.5);
    vec2 fracPart = fract(pixelPos);
    vec2 startTexel = (pixelPos - fracPart) * shadowTexelSize;

    float topleft = sampleShadow(tex, startTexel, compare);
    float topright = sampleShadow(tex, startTexel + vec2(shadowTexelSize.x, 0), compare);
    float bottomleft = sampleShadow(tex, startTexel + vec2(0, shadowTexelSize.y), compare);
    float bottomright = sampleShadow(tex, startTexel + vec2(shadowTexelSize.x, shadowTexelSize.y), compare);

    float top = mix(topleft, topright, fracPart.x);
    float bottom = mix(bottomleft, bottomright, fracPart.x);

    return mix(top, bottom, fracPart.y);
}

bool checkFlag(int flag) {
    return (renderFlags & flag) != 0;
}

void main() {
    vec4 baseColor = checkFlag(ENABLE_TEXTURE) ? texture(diffuseTexture, texCoords) : vec4(1.0);
    if (checkFlag(ENABLE_TELEPORT) && texCoords.x > 0.25 && texCoords.x < 0.75 && texCoords.y > 0.25 && texCoords.y < 0.75) {
        vec2 transformedUv = texCoords * 2.0 - vec2(0.5);
        vec4 tpTileColor = texture(tpTileTexture, tpTileCoords + transformedUv / 16.0) * tileDiffuse;
        baseColor = mix(baseColor, tpTileColor, tpTileColor.w);
    }

    vec3 normal, lightVec, cameraVec;
    if (checkFlag(ENABLE_NORMALS)) {
        // Tangent space
		vec4 normalColor = texture(normalTexture, texCoords);
		vec4 normalValue = 2.0 * normalColor - 1.0;

        normal = normalize(normalValue.rgb);
        normal.y = -normal.y;
        lightVec = normalize(tangentLight);
        cameraVec = normalize(tangentCamera);
    } else {
        // World space
        normal = normalize(worldNormal);
        lightVec = normalize(sun.direction);
        cameraVec = normalize(toCamera);
    }

    vec4 ambient = vec4(sun.ambient * mat.ambient, 1.0);
    vec4 diffuse = vec4(sun.diffuse * mat.diffuse, 1.0);
    vec4 specular = vec4(sun.specular * mat.specular, 1.0);
    vec4 emissive = vec4(mat.emissive, 1.0);

    // Shadow map
    float shadowAmt = checkFlag(ENABLE_SHADOWS) ? shadowLinear(shadowMap, shadowCoords.xy / shadowCoords.w, shadowCoords.z) : 1.0;

    float cosTheta = dot(normal, lightVec) * shadowAmt;
    float diffuseAmt = max(cosTheta, 0.0);
    // Ambient and diffuse lighting
    color = baseColor * diffuse * (ambient + (1.0 - ambient) * diffuseAmt);

    if (cosTheta > 0.0) {
        vec3 reflectedLight = reflect(lightVec, normal);
        float shineAmt = max(dot(cameraVec, reflectedLight), 0.0);
        shineAmt = pow(shineAmt, mat.shininess);

        if (checkFlag(ENABLE_SPECULAR)) {
            specular *= texture(specularTexture, texCoords);
        }

        // Specular lighting
        color += specular * shineAmt * shadowAmt;
    }

    // Emissive lighting
    color += baseColor * emissive;
}
