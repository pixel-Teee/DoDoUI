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
	{

	}
}
