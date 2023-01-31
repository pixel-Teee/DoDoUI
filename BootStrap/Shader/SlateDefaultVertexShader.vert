#version 450

layout(location = 0) in vec4 in_texcoords;
layout(location = 1) in vec2 in_position;
layout(location = 2) in vec4 in_color;
layout(location = 3) in vec4 in_secondary_color;

layout(location = 0) out vec4 texcoords;
layout(location = 1) out vec2 position;
layout(location = 2) out vec4 color;
layout(location = 3) out vec4 secondary_color;

//todo:add uniform or view projection matrix

void main()
{
	texcoords = in_texcoords;

	color = in_color;

	secondary_color = in_secondary_color;

	position = in_position;

	gl_Position = vec4(in_position, 0.0f, 1.0f);
}