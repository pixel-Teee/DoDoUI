#include <PreCompileHeader.h>

#include "SlateFontInfo.h"

namespace DoDo
{
	FFontOutlineSettings FFontOutlineSettings::NoOutline;

	const FCompositeFont* FSlateFontInfo::get_composite_font() const
	{
		if(m_composite_font)
		{
			return m_composite_font.get();
		}

		return nullptr;
	}

	FSlateFontInfo::FSlateFontInfo()
	{
	}
	FSlateFontInfo::FSlateFontInfo(std::shared_ptr<const FCompositeFont> in_composite_font, const int32_t in_size, const DoDoUtf8String& in_type_face_font_name, const FFontOutlineSettings& in_outline_settings)
		: m_font_object(nullptr)
		, m_font_material(nullptr)
		, m_outline_settings(in_outline_settings)
		, m_composite_font(in_composite_font)
		, m_type_face_font_name(in_type_face_font_name)
		, m_size(in_size)
	{
	}
	//static data member
}