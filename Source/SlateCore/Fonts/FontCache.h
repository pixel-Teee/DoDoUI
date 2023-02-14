#pragma once
#include "SlateCore/Textures/TextureAtlas.h"//ISlateAtlasProvider depends on it

namespace DoDo
{
	/*
	 * font caching implementation
	 *
	 * caches characters into texture as needed
	 */
	class FSlateFontCache : public ISlateAtlasProvider
	{
	public:
		/*
		 * constructor
		 *
		 * @param InTextureSize the size of the atlas texture
		 * @param InFontAtlas platform specific font atlas resource
		 */
		//todo:implement ISlateFontAtlasFactory
	};
}
