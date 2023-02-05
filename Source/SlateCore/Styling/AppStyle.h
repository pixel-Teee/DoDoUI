#pragma once

#include "Core/String/DoDoString.h"

namespace DoDo {
	class ISlateStyle;

	class FAppStyle
	{
	public:
		static const ISlateStyle& get();

		static void set_app_style(const ISlateStyle& in_style);
	private:
		static DoDoUtf8String m_appstyle_name;
	};
}