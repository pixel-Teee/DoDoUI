#pragma once
#include "SlateCore/Textures/TextureAtlas.h"//ISlateAtlasProvider depends on it

namespace DoDo
{
	/*information for rendering one glyph in a shaped text sequence*/
	struct FShapedGlyphEntry
	{
		friend class FSlateFontCache;


	};

	/*
	 * manages a potentially large list of non-shaped characters
	 * uses a directly indexed by TCHAR array until space runs out and maps the rest to conserve memory
	 * every character indexed by TCHAR could potentially cost a lot of memory of a lot of empty entries are created
	 * because characters being are far apart
	 */
	class FCharacterList
	{
	public:


	private:
		/*Maintains a fake shaped glyph for each character in the character list*/
		struct FCharacterListEntry
		{
			/*the shaped glyph data for this character*/
			FShapedGlyphEntry m_shaped_glyph_entry;
		};
		/*entries for larger character sets to conserve memory*/

	};
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
