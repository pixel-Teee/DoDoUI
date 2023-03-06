#version 450

//shader types
#define Default			0
#define Border			1
#define GrayScaleFont   2
#define ColorFont		3
#define LineSegment		4
#define RoundedBox		7

layout(location = 0) in vec4 texcoords;
layout(location = 1) in vec2 position;
layout(location = 2) in vec4 color;
layout(location = 3) in vec4 secondary_color;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform ShaderParams{
	vec4 shader_param1;			//16 bytes
	vec4 shader_param2;			//16 bytes
	int shader_type;			//4 bytes
	int ignore_texture_alpha;	//4 bytes
	int disable_effect;			//4 bytes
	int unused[1];				//4 bytes
} shader_param;
layout(set = 0, binding = 1) uniform sampler2D element_texture;

vec4 get_color(vec2 uv)
{
	vec4 final_color;

	vec4 base_color = texture(element_texture, uv);

	final_color = base_color * color;

	return final_color;
}

vec4 get_default_element_color()
{
	return get_color(texcoords.xy + texcoords.zw);
}

float get_rounded_box_distance(vec2 pos, vec2 center, float radius, float inset)
{
	//distance from center
	pos = abs(pos - center);

	//distance from the inner corner
	pos = pos - (center - vec2(radius + inset, radius + inset));

	return mix(length(pos) - radius, max(pos.x - radius, pos.y - radius), float(pos.x <= 0 || pos.y <= 0));
}

vec4 get_rounded_box_element_color()
{
	const vec2 size = shader_param.shader_param1.zw;

	vec2 pos = size * texcoords.xy;

	vec2 center = size / 2.0f;

	//x = top left, y = top right, z = bottom right, w = bottom left
	vec4 corner_radii = shader_param.shader_param2;

	//fingure out which radius to use based on which quadrant we're in
	vec2 quadrant = step(texcoords.xy, vec2(0.5f, 0.5f));

	float left = mix(corner_radii.y, corner_radii.x, quadrant.x);
	float right = mix(corner_radii.z, corner_radii.w, quadrant.x);
	float radius = mix(right, left, quadrant.y);

	float thickness = shader_param.shader_param1.y;

	//compute the distances internal and external to the border outline
	float dext = get_rounded_box_distance(pos, center, radius, 0.0f);
	float din = get_rounded_box_distance(pos, center, max(radius - thickness, 0.0f), thickness);

	//compute the border intensity and fill intensity with a smooth transition
	float spread = 0.5f;
	float bi = smoothstep(spread, -spread, dext);
	float fi = smoothstep(spread, -spread, din);

	//alpha blend the external color
	vec4 fill = get_default_element_color();
	vec4 border = secondary_color;
	vec4 OutColor = mix(border, fill, float(thickness > radius));
	OutColor.a = 0.0;

	// blend in the border and fill colors
	OutColor = mix(OutColor, border, bi);
	OutColor = mix(OutColor, fill, fi);
	return OutColor;
}

void main()
{
	//todo:interms of shader type to get color
	if(shader_param.shader_type == Default)
	{
		out_color = get_default_element_color();
	}
	else if(shader_param.shader_type == RoundedBox)
	{
		out_color = get_rounded_box_element_color();
	}
	//todo:gamma correct

	//vec4 texture_color = texture(element_texture, texcoords.xy + texcoords.zw) * color;

	//out_color = texture_color;
}