#pragma once

#include "Core/Math/IntPoint.h"

namespace DoDo
{
	class ISlateFontAtlasFactory
	{
	public:
		virtual FIntPoint get_atlas_size(const bool is_in_gray_scale) const = 0;

		//virtual std::shared_ptr<FSlateFontAtlas> 
	};
}
