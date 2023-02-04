#include <PreCompileHeader.h>
#include "SlateStyle.h"

namespace DoDo {
	FSlateStyleSet::FSlateStyleSet(const DoDoUtf8String& in_style_set_name)
		: m_style_set_name(in_style_set_name)
	{
	}
	FSlateStyleSet::~FSlateStyleSet()
	{
	}
	void FSlateStyleSet::set_content_root(const DoDoUtf8String& in_content_root_dir)
	{
		m_content_root_dir = in_content_root_dir;//set content root directory
	}
	DoDoUtf8String FSlateStyleSet::root_to_content_dir(const char* relative_path, const char* extension)
	{
		return m_content_root_dir / relative_path;
	}
	const DoDoUtf8String& FSlateStyleSet::get_style_set_name() const
	{
		return m_style_set_name;
	}
}