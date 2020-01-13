#version 410 core

layout(location=0) in vec3 vPosition;

out vec4 clipSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// FOG
out vec4 positionToCamera;

// DUDV MAP
const float tiling = 0.04f;
out vec2 textureCoords;

// FRESNEL
uniform vec3 cameraPos;
out vec3 toCameraVector;

// NORMAL MAP
uniform vec3 lightDir;
out vec3 fromLightVector;

void main()
{
	vec4 position = model * vec4(vPosition, 1.0f);
	positionToCamera = view * position;
	clipSpace = projection * positionToCamera;
    gl_Position = clipSpace;
	
	textureCoords = vec2(position.x / 2.0 + 0.5, position.z / 2.0 + 0.5) * tiling;

	toCameraVector = (cameraPos - position.xyz);

	//fromLightVector = position.xyz - lightDir;
	fromLightVector = -lightDir;
}
