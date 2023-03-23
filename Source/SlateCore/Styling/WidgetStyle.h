#pragma once

#include "Core/Math/Color.h"//FLinearColor depends on it

namespace DoDo
{
	struct FSlateColor;
	/*
	* contains info about those aspects of widget appearance that should be propagated hierarchically
	*/
	class FWidgetStyle
	{
	public:
		/*default constructor*/
		FWidgetStyle()
			: m_color_and_opacity_tint(FLinearColor::White)
			, m_foreground_color(FLinearColor::White)
			, m_subdued_foreground(FLinearColor::White * m_subdued_foreground)//note: * 0.6f
		{

		}

		/*
		* gets the style's foreground color
		* 
		* @return current foreground color
		* @see get color and opacity tint, get subdued foreground color, set foreground color
		*/
		const FLinearColor& get_foreground_color() const
		{
			return m_foreground_color;
		}

		/*
		* gets the style's subdued color
		* 
		* @return current subdued color
		* @see get color and opacity tint, get foreground color
		*/
		const FLinearColor& get_subdued_foreground_color() const
		{
			return m_subdued_foreground;
		}
	private:
		FLinearColor m_color_and_opacity_tint;
		FLinearColor m_foreground_color;
		FLinearColor m_subdued_foreground;

	private:
		static const float m_subdue_amount;
	};
}