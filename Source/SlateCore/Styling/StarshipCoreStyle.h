#pragma once

#include "Core/Core.h"

#include "SlateCore/Layout/Margin.h"

namespace DoDo {
	class ISlateStyle;
	/*
	* core slate style
	*/
	class FStarshipCoreStyle
	{
	public:

		static std::shared_ptr<ISlateStyle> Create();

		static void reset_to_default();//note:this function will call create

		static void set_style(const std::shared_ptr<ISlateStyle>& new_style);

		static const ISlateStyle& get_core_style();
	private:
		class FStyle;

		static void set_up_button_styles(std::shared_ptr<FStyle>& style);

		/*singleton instances of this style*/
		static std::shared_ptr<ISlateStyle> m_instances;
	};
}