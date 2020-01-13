#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 normal;
out vec4 fragPosEye;
out vec4 fragPosLightSpace;
out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightView;

out vec4 worldPosition;
uniform vec4 plane;

// FOG
const float density = 0.0015;
const float gradient = 1.5;
out float visibility;

const vec3 lightPosition = vec3(-1.2f, 1.2f, -0.06f);
out vec3 toLightVector;
out vec3 modelNormal;

void main() 
{
	//compute eye space coordinates
	worldPosition = model * vec4(vPosition, 1.0f);

	gl_ClipDistance[0] = dot(worldPosition, plane);

	fragPosEye = view * worldPosition;
	float distance = length(fragPosEye.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	normal = vNormal;
	fragTexCoords = vTexCoords;
	fragPosLightSpace = lightView * model * vec4(vPosition, 1.0f);
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);

	toLightVector = lightPosition - worldPosition.xyz;
	modelNormal = (model * vec4(vNormal, 0.0f)).xyz;
}
