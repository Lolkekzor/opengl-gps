#version 410 core

in vec4 clipSpace;

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

// DUDVMAP
in vec2 textureCoords;
uniform sampler2D dudvMap;
const float waveStrength = 0.02;

uniform float moveFactor;
const float waveSpeed = 0.04;

// FRESNEL
in vec3 toCameraVector;

// NORMAL MAP
uniform sampler2D normalMap;
uniform vec3 lightColor;
in vec3 fromLightVector;

const float shineDamper = 20.0;
const float reflectivity = 0.6;

// FOG
in vec4 positionToCamera;
vec3 skyColor = vec3(0.44,0.53,0.6); // +0.0,0.10,0.15

const float density = 0.0015;
const float gradient = 1.5;

void main(void) {
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	float distortFactor = moveFactor * waveSpeed;

	vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + distortFactor, textureCoords.y)).rg*0.1;
	distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y+distortFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

	//vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x + distortFactor, textureCoords.y)).rg * 2.0 - 1.0) * waveStrength;
	//vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x + distortFactor, textureCoords.y + distortFactor)).rg * 2.0 - 1.0) * waveStrength;
	//vec2 totalDistortion = distortion1 + distortion2;

	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

	vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractColor = texture(refractionTexture, refractTexCoords);

	float distance = length(positionToCamera.xyz);
	float visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));
	refractiveFactor = pow(refractiveFactor, 2);

	vec4 normalMapColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColor * specular * reflectivity;

	vec4 waterColor = mix(reflectColor, refractColor, refractiveFactor);
	waterColor = mix(waterColor, vec4(0.05, 0.55, 0.85, 1), 0.2);
	out_Color = mix(vec4(skyColor, 1.0), waterColor, visibility) + vec4(specularHighlights, 0.0f);
}