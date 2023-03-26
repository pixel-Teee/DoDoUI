#pragma once

#include "SlateCore/Styling/SlateBrush.h"

namespace DoDo {
	/*
	* a 3x3 box where the sides stretch horizontally and vertically and the middle stretches to fill
	* the corners will not be stretched, the size of the sides and corners is determined by the margin as follows
	* 
	*                 _____________________
	*                |  | Margin.Top    |  |
	*                |__|_______________|__|   Margin.Right
	*                |  |               |  |  /
	*              +--> |               | <--+
	*             /  |__|_______________|__|
	*  Margin.Left   |  | Margin.Bottom |  |
	*                |__|_______________|__|
	*/
	struct FSlateBoxBrush : public FSlateBrush
	{
		/*
		* make a 3x3 box that stretches the texture
		* 
		* @param InImageName the name of image to make into a box
		* @param InMargin the size of corners and sides in normalized texture UV space
		* @param InColorAndOpacity color and opacity scale
		*/
		inline FSlateBoxBrush(const DoDoUtf8String& in_image_name, const FMargin& in_margin, const FLinearColor& in_color_and_opacity = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushImageType::Type in_image_type = ESlateBrushImageType::FullColor)
			: FSlateBrush(ESlateBrushDrawType::Box, in_image_name, in_margin, ESlateBrushTileType::NoTile, in_image_type, glm::vec2(0.0f), in_color_and_opacity)
		{}

		inline FSlateBoxBrush(const DoDoUtf8String& in_image_name, const FMargin& in_margin, const FSlateColor& in_color_and_opacity, ESlateBrushImageType::Type in_image_type = ESlateBrushImageType::FullColor)
			: FSlateBrush(ESlateBrushDrawType::Box, in_image_name, in_margin, ESlateBrushTileType::NoTile, in_image_type, glm::vec2(0.0f), in_color_and_opacity)
		{}
	};
}