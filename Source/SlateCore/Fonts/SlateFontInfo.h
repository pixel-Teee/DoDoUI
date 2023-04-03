#pragma once
#include "Core/String/DoDoString.h"

#include "Core/Templates/TypeHash.h"

#include "CompositeFont.h"

#include <glm/glm.hpp>//glm::vec4

namespace DoDo
{
	/*
	 * sets the maximum font fallback level, for when a character can't be found in the selected font set
	 *
	 */
	enum class EFontFallback : uint8_t
	{
		/*no fallback font*/
		FF_NoFallback,

		FF_Max
	};
	/*
	 * settings for applying an outline to a font
	 */
	struct FFontOutlineSettings
	{
		/*size of the outline in slate units (at 1.0 font scale this unit is a pixel)*/
		int32_t m_outline_size;

		/*the color of the outline for any character in this font*/
		glm::vec4 m_outline_color;

		static FFontOutlineSettings NoOutline;

		bool is_identical_to_for_caching(const FFontOutlineSettings& other) const
		{
			return m_outline_size == other.m_outline_size;
		}

		friend inline uint32_t Get_Type_Hash(const FFontOutlineSettings& outline_settings)
		{
			uint32_t hash = 0;
			hash = Hash_Combine(hash, Get_Type_Hash(outline_settings.m_outline_size));
			return hash;
		}
	};
	/*
	 * a representation of a font in slate
	 */
	struct FSlateFontInfo
	{
		/*the font object (valid when used from umg or a slate widget style asset)*/
		void* m_font_object;

		//todo:add material
		/*the material to use when rendering this font*/
		void* m_font_material;

		/*settings for applying an outline to a font*/
		FFontOutlineSettings m_outline_settings;

		//todo:implement FCompositeFont
		std::shared_ptr<const FCompositeFont> m_composite_font;

		/*the name of the font to use from the default typeface (None will use the first entry)*/
		DoDoUtf8String m_type_face_font_name;

		/*
		 * the font size is a measure in point values, the conversion of points to slate units is done at 96 dpi,
		 * so if you're using a tool like photoshop or prototype layouts and UI mock ups, be sure to change the default dpi
		 * measurements from 72 dpi to 96 dpi
		 */
		int32_t m_size;

		/*the uniform spacing (or tracking) between all characters in the text*/
		int32_t m_letter_spacing = 0;

		EFontFallback m_font_fallback;

		/*
		 * get the composite font pointer associated with this font info
		 */
		const FCompositeFont* get_composite_font() const;

	public:
		FSlateFontInfo();
		/*
		* creates and initializes a new instance with the specified font, size, and emphasis
		* 
		* @param InCompositeFont the font instance to use
		* @param InSize the size of the font
		* @param InTypefaceFontName the name of the font to use from the default typeface (none will use the first entry)
		*/
		FSlateFontInfo(std::shared_ptr<const FCompositeFont> in_composite_font, const int32_t in_size, const DoDoUtf8String& in_type_face_font_name = "",
			const FFontOutlineSettings& in_outline_settings = FFontOutlineSettings());

		bool is_identical_to_for_caching(const FSlateFontInfo& other) const
		{
			//todo:ignore composite font
			//ignore font material because it does not affect the cached glyph
			return m_font_object == other.m_font_object
				&& m_outline_settings.is_identical_to_for_caching(other.m_outline_settings)
				&& m_type_face_font_name == other.m_type_face_font_name
				&& m_size == other.m_size;
		}

		friend inline uint32_t Get_Type_Hash(const FSlateFontInfo& font_info)
		{
			//ignore font material because it does not affect the cached glyph
			uint32_t hash = 0;
			hash = Hash_Combine(hash, Get_Type_Hash((uint64_t)font_info.m_font_object));
			hash = Hash_Combine(hash, Get_Type_Hash(font_info.m_outline_settings));
			//hash = Hash_Combine(hash, Get_Type_Hash(font_info.m_type_face_font_name));
			//todo:implement m_type_face_font_name hash
			hash = Hash_Combine(hash, Get_Type_Hash(font_info.m_size));

			return hash;
		}

		inline bool operator==(const FSlateFontInfo& other) const
		{
			return is_identical_to_for_caching(other);
		}
	};
}
