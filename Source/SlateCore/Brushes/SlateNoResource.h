#pragma once

#include "SlateCore/Styling/SlateBrush.h"

namespace DoDo
{
	/*
	 * a resource that has no appearance
	 */
	struct FSlateNoResource : public FSlateBrush
	{
		/*
		 * creates and initializes a new instance
		 *
		 * @param InImageSize an optional image size (default is zero)
		 */
		FSlateNoResource(const glm::vec2& in_image_size = glm::vec2(0.0f))
			: FSlateBrush(ESlateBrushDrawType::NoDrawType, "", FMargin(0.0f), ESlateBrushTileType::NoTile, ESlateBrushImageType::NoImage, in_image_size)
		{}
	};
}
