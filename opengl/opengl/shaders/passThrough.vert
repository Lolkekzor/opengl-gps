#version 330

layout(location = 0) in vec3 in_position;

out vec2 UV;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(in_position, 1.0);
	UV = (in_position.xy+vec2(1,1))/2.0;
}