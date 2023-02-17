#pragma once

#include "Core/Math/IntPoint.h"

#include "SlateFontInfo.h"//FSlateFontKey's FSlateFontInfo depends on it

namespace DoDo
{
	struct FSlateFontKey
	{
	public:

	private:
		FSlateFontInfo m_font_info;//font info


	};

	class ISlateFontAtlasFactory
	{
	public:
		virtual FIntPoint get_atlas_size(const bool is_in_gray_scale) const = 0;

		//virtual std::shared_ptr<FSlateFontAtlas> 
	};
}
