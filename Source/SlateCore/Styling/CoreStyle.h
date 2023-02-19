#pragma once

#include "Core/Core.h"

#include "SlateCore/Layout/Margin.h"

#include "AppStyle.h"

namespace DoDo {
	class ISlateStyle;
	struct FCompositeFont;
	/*
	* core slate style
	*/
	class FCoreStyle
	{
	public:

		static bool is_star_ship_style();

		static const ISlateStyle& get()
		{
			return FAppStyle::get();
		}

		static const ISlateStyle& get_core_style()
		{
			return *(m_instance.get());
		}

		/*get the default font for slate*/
		static std::shared_ptr<const FCompositeFont> get_default_font();

	private:
		/*singleton instances of this style*/
		static std::shared_ptr<ISlateStyle> m_instance;//note:not use this
	};

	namespace CoreStyleConstants
	{
		const glm::vec2 Icon16x16(16.0f, 16.0f);

		//common margins
		const FMargin default_margins(0.8f, 4.0f);

		//buttons already have a built in (4., 2.) padding - adding to that a little
		const FMargin button_margins(12.0f, 1.5f, 12.f, 1.5f);

		const FMargin pressed_button_margins(12.0f, 2.5f, 12.0f, 0.5f);

		const float input_focus_thickness = 1.0f;
	}
}