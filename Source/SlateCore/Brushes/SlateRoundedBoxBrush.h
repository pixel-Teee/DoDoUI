#pragma once

#include "SlateCore/Styling/SlateBrush.h"

namespace DoDo {
	struct FSlateRoundedBoxBrush
		: public FSlateBrush
	{
		/*
		* creates and initializes a new instance with the specified color and rounds based on height
		* 
		* @param InColor linear fill color
		* @param InRadius corner radius in slate units applied to the outline at each corner, x = top left, y = top right, z = bottom right, w = bottom left
		* @param InOutlineColor Outline Color
		* @param InOutlineWidth Outline Width or Thickness
		*/
		template<typename FillColorType, typename OutlineColorType, typename RadiusType>
		FSlateRoundedBoxBrush(const FillColorType& in_color, RadiusType in_radius, const OutlineColorType& in_outline_color, float in_outline_width, const glm::vec2& in_image_size = glm::vec2(0.0f, 0.0f))
			: FSlateBrush(ESlateBrushDrawType::RoundedBox,
				"",
				FMargin(0.0f),
				ESlateBrushTileType::NoTile,
				ESlateBrushImageType::NoImage,
				in_image_size,
				in_color,
				nullptr,
				false)
		{
			//set outline settings
		}


	};
}