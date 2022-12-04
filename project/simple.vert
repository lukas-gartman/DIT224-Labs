#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 in_data;

uniform mat4 modelViewProjectionMatrix;

out vec4 data;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
	data = in_data;
}
