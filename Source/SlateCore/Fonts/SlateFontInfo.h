#pragma once
#include "Core/String/DoDoString.h"

namespace DoDo
{
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

		//todo:implement FCompositeFont

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
	};
}