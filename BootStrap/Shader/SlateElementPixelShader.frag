#version 450

layout(location = 0) in vec4 texcoords;
layout(location = 1) in vec2 position;
layout(location = 2) in vec4 color;
layout(location = 3) in vec4 secondary_color;

layout(location = 0) out vec4 out_color;

void main()
{
	//todo:interms of shader type to get color

	//todo:gamma correct

	out_color = color;
}