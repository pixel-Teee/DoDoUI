#version 450

layout(location = 0) in vec4 texcoords;
layout(location = 1) in vec2 position;
layout(location = 2) in vec4 color;
layout(location = 3) in vec4 secondary_color;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform ShaderParams{
	vec4 shader_param1;
	vec4 shader_param2;
	int shader_type;
} shader_param;
layout(set = 0, binding = 1) uniform sampler2D element_texture;

void main()
{
	//todo:interms of shader type to get color

	//todo:gamma correct

	vec3 texture_color = texture(element_texture, texcoords.xy * texcoords.zw);

	out_color = color;
}