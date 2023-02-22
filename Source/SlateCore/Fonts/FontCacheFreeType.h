#pragma once

#include "CompositeFont.h"//FFontFaceDataConstPtr
#include "Core/String/DoDoString.h"//DoDoUtf8String
#include <set>
#include "freetype/freetype.h"

namespace DoDo
{
	namespace FreeTypeUtils
	{
		/*
		* load the given glyph into the active slot of the given face
		*/
		FT_Error load_glyph(FT_Face in_face, const uint32_t in_glyph_index, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale);
	}
	/*
	 * wrapped around a free type library instance
	 * this instance will be create using our memory allocator
	 */
	class FFreeTypeLibrary
	{
	public:
		FFreeTypeLibrary();
		~FFreeTypeLibrary();

		FT_Library get_library() const
		{
			return m_ft_library;
		}

	private:
		//non-copyable
		FFreeTypeLibrary(const FFreeTypeLibrary&);
		FFreeTypeLibrary& operator=(const FFreeTypeLibrary&);

		FT_Library m_ft_library;
		//FT_Memory m_custom_memory;
	};
	/*
	 * wrapped around a free type face instance
	 * it will either steal the given buffer, or stream the given file from disk
	 */
	class FFreeTypeFace
	{
	public:

		float get_bit_map_render_scale() const
		{
			//todo:implement free type utils
			//return FreeTypeUtils::get_bit_map_render_scale(m_ft_face);

			return 1.0f;
		}

		bool is_face_valid()
		{
			return m_ft_face != nullptr;
		}

		FT_Face get_face() const
		{
			return m_ft_face;
		}
	private:
		//non-copyable
		FFreeTypeFace(const FFreeTypeFace&);
		FFreeTypeFace& operator=(const FFreeTypeFace&);

		FT_Face m_ft_face;//free type face

		FFontFaceDataConstPtr m_memory;//just a uint8_t memory block

		std::set<DoDoUtf8String>  m_attributes;

		EFontLayoutMethod m_layout_method;
	};
}