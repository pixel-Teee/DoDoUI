#include <PreCompileHeader.h>

#include "SlateBrush.h"

namespace DoDo
{
	FSlateBrush::FSlateBrush()
	{
	}

	FSlateBrush::~FSlateBrush()
	{
	}
	//todo:called by Slate Rounded Box Brush
	FSlateBrush::FSlateBrush(ESlateBrushDrawType::Type in_draw_type, 
		const DoDoUtf8String in_source_name, const FMargin& in_margin, 
		ESlateBrushTileType::Type in_tiling, ESlateBrushImageType::Type in_image_type, 
		const glm::vec2& in_image_size, const glm::vec4& in_tint, void* in_object_resource, bool b_in_dynamically_loaded)
		: m_image_size(in_image_size)
		, m_margin(in_margin)
		, m_tint_color(in_tint)
		, m_resource_object((Object*)in_object_resource)//todo:fix me
		, m_resource_name(in_source_name)
		, m_uv_region(ForceInit)
		, m_draw_as(in_draw_type)
		, m_tiling(in_tiling)
		, m_mirroring(ESlateBrushMirrorType::NoMirror)
		, m_image_type(in_image_type)
	{

	}
}
