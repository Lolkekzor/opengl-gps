#version 410 core

layout(location=0) in vec3 vPosition;

uniform mat4 lightView;
uniform mat4 model;

void main()
{
    gl_Position = lightView * model * vec4(vPosition, 1.0f);
}
