#pragma once

#include "FontCacheFreeType.h"

#include "Core/Internationlization/Text.h"

#include "SlateCore/Fonts/ShapedTextFwd.h"

namespace DoDo
{
	/*
	 * internal class used to hold the FreeType data within the shaped glyph sequence
	 */
	class FShapedGlyphFaceData
	{
	public:
		FShapedGlyphFaceData(std::weak_ptr<FFreeTypeFace> in_font_face, const uint32_t in_glyph_flags, const int32_t in_font_size, const float in_font_scale)
			: m_font_face(std::move(in_font_face))
			, m_glyph_flags(in_glyph_flags)
			, m_font_size(in_font_size)
			, m_font_scale(in_font_scale)
			, m_bit_map_render_scale(1.0f)
		{
			std::shared_ptr<FFreeTypeFace> font_face_pin = m_font_face.lock();

			if (font_face_pin)
			{
				//m_bit_map_render_scale = font_face_pin->get_bitmap_render_scale();
				m_bit_map_render_scale = 1.0f;
			}
		}
	
		/*weak pointer to the FreeType face to render with*/
		std::weak_ptr<FFreeTypeFace> m_font_face;
		/*provides the glyph flags used to render the font*/
		uint32_t m_glyph_flags;
		/*provides the point size used to render the font*/
		int32_t m_font_size;
		/*provides the final scale used to render to the font*/
		float m_font_scale;
		/*any additional scale that should be applied when rendering this glyph*/
		float m_bit_map_render_scale;
	};

	/*
	* bridging point between harfbuzz and the slate font system
	* this class, via the instances you pass to its constructor, knows how to correctly shape a slate font
	*/
	class FCompositeFontCache;
	class FSlateFontRenderer;
	class FSlateFontCache;
	class FSlateFontInfo;
	enum class ETextShapingMethod : uint8_t;
	class FSlateTextShaper
	{
	public:
		FSlateTextShaper(FFreeTypeCacheDirectory* in_ft_cache_directory, FCompositeFontCache* in_composite_font_cache, FSlateFontRenderer* in_font_renderer, FSlateFontCache* in_font_cache);

		FShapedGlyphSequencePtr shape_bidirectional_text(const DoDoUtf8String& in_text, const int32_t in_text_start, const int32_t in_text_len, const FSlateFontInfo& in_font_info,
			const float in_font_scale, const TextBiDi::ETextDirection in_base_direction, const ETextShapingMethod text_shaping_method) const;
	private:

		FFreeTypeCacheDirectory* m_ft_cache_directory;
		FCompositeFontCache* m_composite_font_cache;
		FSlateFontRenderer* m_font_renderer;
		FSlateFontCache* m_font_cache;

		/*unicode bidi text detection*/
		//todo:add TextBiDi::ITextBiDi

		//todo:add IBreakIterator

		/*harfbuzz font factory(using our cached functions rather that free type directly)*/

	};
}