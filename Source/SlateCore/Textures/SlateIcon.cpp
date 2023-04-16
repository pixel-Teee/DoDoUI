#include <PreCompileHeader.h>

#include "SlateIcon.h"

#include "SlateCore/Styling/ISlateStyle.h"//ISlateStyle depends on it

#include "SlateCore/Styling/SlateStyleRegistry.h"//FSlateStyleRegistry depends on it

#include "SlateCore/Styling/SlateDefaults.h"//FStyleDefaults depends on it

namespace DoDo {
	FSlateIcon::FSlateIcon()
		: m_style_set_name("")
		, m_style_name("")
		, m_small_style_name("")
		, m_status_overlay_style_name("")
	{
	}
	FSlateIcon::FSlateIcon(const DoDoUtf8String in_style_set_name, const DoDoUtf8String in_style_name, const DoDoUtf8String in_small_style_name, const DoDoUtf8String in_status_overlay_style_name)
		: m_style_set_name(in_style_set_name)
		, m_style_name(in_style_name)
		, m_small_style_name((in_small_style_name == "") ? (in_style_name + ".Small") : in_small_style_name)
		, m_status_overlay_style_name(in_status_overlay_style_name)
	{
	}
	const FSlateBrush* FSlateIcon::get_icon() const
	{
		const ISlateStyle* style_set = get_style_set();

		if (style_set)
		{
			return style_set->get_optional_brush(m_style_name);
		}

		return FStyleDefaults::get_no_brush();
	}
	const ISlateStyle* FSlateIcon::get_style_set() const
	{
		return m_style_set_name.get_length() == 0 ? nullptr : FSlateStyleRegistry::find_slate_style(m_style_set_name);
	}
}