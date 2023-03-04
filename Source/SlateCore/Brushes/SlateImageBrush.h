#pragma once

#include "SlateCore/Styling/SlateBrush.h"

namespace DoDo {
	class FSlateImageBrush : public FSlateBrush
	{
	public:
		FSlateImageBrush(const DoDoUtf8String& in_image_name, const glm::vec2& in_image_size, const glm::vec4& in_tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		ESlateBrushTileType::Type in_tiling = ESlateBrushTileType::NoTile, ESlateBrushImageType::Type in_image_type = ESlateBrushImageType::FullColor
		) : FSlateBrush(ESlateBrushDrawType::Image, in_image_name, FMargin(0.0f), in_tiling, in_image_type, in_image_size, in_tint)
		{}
	};

	struct FSlateVectorImageBrush : public FSlateImageBrush
	{
		FSlateVectorImageBrush(const DoDoUtf8String& in_image_name, const glm::vec2& in_image_size, const glm::vec4& in_tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::Type in_tiling =
		ESlateBrushTileType::NoTile) : FSlateImageBrush(in_image_name, in_image_size, in_tint, in_tiling, ESlateBrushImageType::Vector)
		{}

	};
}