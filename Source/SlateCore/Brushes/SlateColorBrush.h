#pragma once

#include "SlateCore/Styling/SlateBrush.h"

namespace DoDo {
	/*
	* a color brush, draws a box with no margins that only has a color applied to it
	*/
	struct FSlateColorBrush : public FSlateBrush
	{
		/*
		* creates and initializes a new instance with the specified linear color
		* 
		* @param InColor the linear color to use
		*/
		FSlateColorBrush(const glm::vec4& in_color)
			: FSlateBrush(ESlateBrushDrawType::Image, "", FMargin(0.0f), ESlateBrushTileType::NoTile, ESlateBrushImageType::NoImage, glm::vec2(0.0f, 0.0f), in_color)
		{

		}
	};
}