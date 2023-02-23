#include <PreCompileHeader.h>

#include "CompositeFont.h"

namespace DoDo
{
	FFontData::FFontData()
	{
	}
	FFontData::FFontData(DoDoUtf8String in_font_file_name, const EFontHinting in_hinting, const EFontLoadingPolicy in_loading_policy, const int32_t in_sub_face_index)
		: m_font_file_name(std::move(in_font_file_name))
	{
		//m_font_file_name_hash = Get_Type_Hash(m_font_file_name);
		std::string file_name = std::string(m_font_file_name.c_str());
		auto hasher = std::hash<std::string>();

		m_font_file_name_hash = hasher(file_name);//hash this
	}

	const DoDoUtf8String& FFontData::get_font_file_name() const
	{
		return m_font_file_name;
	}

	int32_t FFontData::get_sub_face_index() const
	{
		return m_sub_face_index;
	}

	bool FFontData::operator==(const FFontData& other) const
	{
		if (this == &other)
			return true;

		return m_font_file_name_hash == other.m_font_file_name_hash
			&& m_sub_face_index == other.m_sub_face_index
			&& m_font_file_name == other.m_font_file_name;
	}
}
