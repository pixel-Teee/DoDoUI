#include <PreCompileHeader.h>

#include "AppStyle.h"

#include "ISlateStyle.h"

#include "SlateStyleRegistry.h"

#include "CoreStyle.h"

namespace DoDo {
	DoDoUtf8String FAppStyle::m_appstyle_name;
	const ISlateStyle& FAppStyle::get()
	{
		//to style registry to find style
		if (const ISlateStyle* app_style = FSlateStyleRegistry::find_slate_style(m_appstyle_name))
		{
			return *app_style;
		}

		return FCoreStyle::get_core_style();
		
	}
	void FAppStyle::set_app_style(const ISlateStyle& in_style)
	{
		m_appstyle_name = in_style.get_style_set_name();
	}
}