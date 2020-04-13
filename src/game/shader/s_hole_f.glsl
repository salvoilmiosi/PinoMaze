#version 330 core

in vec2 texCoords;
in vec4 clipSpace;
in vec3 toCamera;

out vec4 color;

uniform sampler2D refractionTexture;
uniform sampler2D normalTexture;
uniform sampler2D dudvTexture;

uniform vec3 lightDirection;
uniform float shininess = 10.0;

uniform float globalTime;

void main() {
    vec3 uv_proj = (clipSpace.xyz / clipSpace.w) * 0.5 + 0.5;

    float moveFactor = globalTime * 0.0001;

    vec2 offset = texture2D(dudvTexture, texCoords + vec2(moveFactor, 0.0)).rg * 0.1;
    offset += texCoords + vec2(0.0, moveFactor);
    vec2 dudv = (texture2D(dudvTexture, offset).rg * 2.0 - 1.0) * 0.05;

    color = texture2D(refractionTexture, uv_proj.xy + dudv);
	
	vec3 normalColor = texture2D(normalTexture, offset).xyz;
	vec3 normal = normalColor.rbg * 2.0 - 1.0;

	// Light direction in world space
    vec3 reflectedLight = reflect(normalize(lightDirection), normalize(normal));

    float shineAmt = max(dot(normalize(toCamera), reflectedLight), 0.0);
    shineAmt = pow(shineAmt, shininess);

    color += vec4(0.4, 0.4, 0.4, 1.0) * shineAmt;
}

