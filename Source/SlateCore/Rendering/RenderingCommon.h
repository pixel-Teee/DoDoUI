#pragma once

#include "Core/Math/TransformCalculus2D.h"//slate vertex depends on it
#include "Core/Misc/EnumClassFlags.h"//ENUM_CLASS_FLAGS
#include "SlateRenderTransform.h"

#include "SlateCore/Styling/SlateColor.h"//FSlateColor depends on it

//#include <vulkan/vulkan.h>

namespace DoDo
{
	//struct VertexInputDesription
	//{
	//	std::vector<VkVertexInputBindingDescription> m_bindings;
	//	std::vector<VkVertexInputAttributeDescription> m_attributes;//every element
	//
	//	VkPipelineVertexInputStateCreateFlags flags = 0;
	//};

	class FSlateDrawElement;//forward declare

	/*
	* shader types, note: mirrored in the shader file
	* if you add a type here, you must also implement the proper shader type (TSlateElementPS), see slate shaders.h
	*/
	enum class ESlateShader : uint8_t
	{
		/*the default shader type, simple texture lookup*/
		Default = 0,
		/*border shader*/
		Border = 1,
		/*grayscale font shader, uses an alpha only texture*/
		GrayScaleFont = 2,
		/*grayscale font shader, uses an color texture*/
		ColorFont = 3,
		/*line segment shader, for drawing anti-aliased lines*/
		LineSegment = 4,
		/*for completely customized materials, makes no assumptions on use*/
		Custom = 5,
		/*for post processing passes*/
		PostProcess = 6,
		/*rounded box shader*/
		RoundedBox = 7
	};

	/*
	 * draw primitive types
	 */
	enum class ESlateDrawPrimitive : uint8_t
	{
		None,
		LineList,
		TriangleList
	};

	enum class ESlateVertexRounding : uint8_t
	{
		Disabled,
		Enabled
	};
	/*a struct which defines a basic vertex seen by the slate vertex buffers and shaders*/
	struct FSlateVertex
	{
		/*texture coordinates, the first 2 are in xy, and the 2nd are in zw*/
		float tex_coords[4];

		/*texture coordinates used as pass through to materials for custom texturing*/
		//todo:implement this

		/*position of the vertex in window space*/
		glm::vec2 m_position;

		/*vertex color*/
		FColor m_color;

		/*secondary vertex color, generally used for outlines*/
		FColor m_secondary_color;

		/*local size of the element*/
		uint16_t m_pixel_size[2];

		FSlateVertex(){}

		//static VertexInputDesription get_vertex_description();

	public:

		template<ESlateVertexRounding Rounding>
		static FSlateVertex Make(const FSlateRenderTransform& render_transform, const glm::vec2 in_local_position, const glm::vec4 in_tex_coord, const glm::vec2 in_tex_coord2, const FColor in_color, const FColor secondary_color = FColor())
		{
			FSlateVertex vertex;
			vertex.tex_coords[0] = in_tex_coord.x;
			vertex.tex_coords[1] = in_tex_coord.y;
			vertex.tex_coords[2] = in_tex_coord.z;
			vertex.tex_coords[3] = in_tex_coord.w;
			vertex.init_common<Rounding>(render_transform, in_local_position, in_color, secondary_color);

			return vertex;
		}

		template<ESlateVertexRounding Rounding>
		static FSlateVertex Make(const FSlateRenderTransform& render_transform, const glm::vec2 in_local_position, const glm::vec2 in_local_size, float scale, const glm::vec4 in_tex_coords, const FColor& in_color, const FColor in_secondary_color)
		{
			FSlateVertex vertex;
			vertex.tex_coords[0] = in_tex_coords.x;
			vertex.tex_coords[1] = in_tex_coords.y;
			vertex.tex_coords[2] = in_tex_coords.z;
			vertex.tex_coords[3] = in_tex_coords.w;
			//todo:populate material tex coordinates
			vertex.init_common<Rounding>(render_transform, in_local_position, in_color, in_secondary_color);

			const int32_t pixel_size_x = in_local_size.x * scale;
			const int32_t pixel_size_y = in_local_size.y * scale;
			vertex.m_pixel_size[0] = (uint16_t)pixel_size_x;
			vertex.m_pixel_size[1] = (uint16_t)pixel_size_y;

			return vertex;
		}

	private:
		template<ESlateVertexRounding Rounding>
		void init_common(const FSlateRenderTransform& render_transform, const glm::vec2 in_local_position, const FColor in_color, const FColor in_secondary_color)
		{
			//todo:implement transform point
			m_position = transform_point(render_transform, in_local_position);

			if(Rounding == ESlateVertexRounding::Enabled)
			{
				//todo:implement round
			}

			m_color = in_color;
			m_secondary_color = in_secondary_color;
		}

		
	};

	/*
	VertexInputDesription FSlateVertex::get_vertex_description()
	{
		VertexInputDesription description;

		//we will have just 1 vertex buffer binding, with a pre-vertex rate
		VkVertexInputBindingDescription main_binding = {};
		main_binding.binding = 0;
		main_binding.stride = sizeof(FSlateVertex);
		main_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		description.m_bindings.push_back(main_binding);

		//texcoord will be stored at location 0
		VkVertexInputAttributeDescription texcoord_attribute = {};
		texcoord_attribute.binding = 0;
		texcoord_attribute.location = 0;
		texcoord_attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;//texcoord
		texcoord_attribute.offset = offsetof(FSlateVertex, tex_coords);

		VkVertexInputAttributeDescription position_attribute = {};
		position_attribute.binding = 0;
		position_attribute.location = 1;
		position_attribute.format = VK_FORMAT_R32G32_SFLOAT;//position
		position_attribute.offset = offsetof(FSlateVertex, m_position);

		VkVertexInputAttributeDescription color_attribute = {};
		color_attribute.binding = 0;
		color_attribute.location = 2;
		color_attribute.format = VK_FORMAT_B8G8R8A8_UNORM;
		color_attribute.offset = offsetof(FSlateVertex, m_color);

		VkVertexInputAttributeDescription color2_attribute = {};
		color2_attribute.binding = 0;
		color2_attribute.location = 3;
		color2_attribute.format = VK_FORMAT_B8G8R8A8_UNORM;
		color2_attribute.offset = offsetof(FSlateVertex, m_secondary_color);

		//every element
		description.m_attributes.push_back(texcoord_attribute);
		description.m_attributes.push_back(position_attribute);
		description.m_attributes.push_back(color_attribute);
		description.m_attributes.push_back(color2_attribute);

		return description;
	}
	*/

	/*
	 * effects that can be applied to elements when rendered
	 * note : new effects added should be in mask form
	 * if you add a type here you must also implement the proper shader type(TSlateElementPS), see slate shaders.h
	 */
	enum class ESlateDrawEffect
	{
		/*no effect applied*/
		None = 0,
		/*advanced : draw the element with no blending*/
		NoBlending = 1 << 0,
		/*advanced : blend using pre-multiplied alpha, ignored if no blending is set*/
		PreMultipliedAlpha = 1 << 1,
		/*advanced : no gamma correction should be done*/
		NoGamma = 1 << 2,
		/*advanced : change the alpha value to 1 - Alpha*/
		InvertAlpha = 1 << 3,

		/*draw the element with a disabled effect*/
		DisabledEffect = 1 << 5
	};

	/*
	* shader parameters for slate
	*/
	struct FShaderParams
	{
		/*pixel shader parameters*/
		glm::vec4 m_pixel_params;
		glm::vec4 m_pixel_params2;
		glm::vec4 m_pixel_params3;

		FShaderParams()
			: m_pixel_params(0.0f)
			, m_pixel_params2(0.0f)
			, m_pixel_params3(0.0f)
		{}

		FShaderParams(const glm::vec4& in_pixel_params, const glm::vec4& in_pixel_params2 = glm::vec4(0.0f), const glm::vec4& in_pixel_params3 = glm::vec4(0.0f))
			: m_pixel_params(in_pixel_params)
			, m_pixel_params2(in_pixel_params2)
			, m_pixel_params3(in_pixel_params3)
		{}

		bool operator==(const FShaderParams& other) const
		{
			return m_pixel_params == other.m_pixel_params && m_pixel_params2 == other.m_pixel_params2 && m_pixel_params3 == other.m_pixel_params3;
		}

		static FShaderParams make_pixel_shader_params(const glm::vec4& pixel_shader_params, const glm::vec4& in_pixel_shader_params2 = glm::vec4(0.0f),
			const glm::vec4& in_pixel_shader_param3 = glm::vec4(0.0f))
		{
			return FShaderParams(pixel_shader_params, in_pixel_shader_params2, in_pixel_shader_param3);
		}
	};

	ENUM_CLASS_FLAGS(ESlateDrawEffect)

	typedef std::vector<FSlateDrawElement> FSlateDrawElementArray;
	typedef std::vector<FSlateVertex> FSlateVertexArray;
	typedef std::vector<uint16_t> FSlateIndexArray;
}
