#pragma once

namespace DoDo
{
	struct FSlateFontInfo;
	/*
	 * bridging point between FreeType and the Slate font system
	 * this class, via the instances you pass to its constructor, knows how to correctly render a slate font
	 */
	class FSlateFontRenderer
	{
	public:
		/*
		 * @return the global max height for any character in the default font
		 */
		uint16_t get_max_height(const FSlateFontInfo& in_font_info, const float in_scale) const;

		//todo:implement FFreeTypeLibrary, FFreeTypeCacheDirectory, FCompositeFontCache
	};
}