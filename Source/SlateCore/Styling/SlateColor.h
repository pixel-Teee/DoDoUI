#pragma once

#include "Core/Core.h"

#include "glm/glm.hpp"

#include "Core/Math/Color.h"//FLinearColor

#include "WidgetStyle.h"//FWidgetStyle depends on it

namespace DoDo {
	enum class ESlateColorStylingMode : uint8_t
	{
		use_color_specified,

		use_color_color_table,

		use_color_foreground,//use the widget's foreground color

		use_color_foreground_subdued,//use the widget's subdued color

		use_color_use_style//use the foreground color defined in a widget specific style
	};

	enum class EStyleColor : uint8_t;//color table id

	/*
	* a slate color can be a directly specified value, or the color can be pulled from a widget style
	*/
	struct FSlateColor
	{
	public:
		FSlateColor()
			: m_specified_color(FLinearColor(1.0f, 0.0f, 1.0f))
			, m_color_use_rule(ESlateColorStylingMode::use_color_specified)
		{}

		FSlateColor(const FLinearColor& in_color)
			: m_specified_color(in_color)
			, m_color_use_rule(ESlateColorStylingMode::use_color_specified)
		{}

		/*
		* creates a new slate color with the specified values
		* 
		* @param InColor the color value to assign
		*/
		FSlateColor(const FColor in_color)
			: m_specified_color(in_color.reinterpret_as_linear())//note:this color will not conversion, just to divide 255
			, m_color_use_rule(ESlateColorStylingMode::use_color_specified)
		{}

		FSlateColor(EStyleColor in_color_table_id)
			: m_specified_color()
			, m_color_use_rule(ESlateColorStylingMode::use_color_color_table)//note:just use a color table
			, m_color_table_id(in_color_table_id)
		{}

		~FSlateColor()
		{}

	public:
		/*
		* gets the color value represented by this slate color
		* 
		* @param InWidgetStyle the widget style to use when this color represents a foreground or subdued color
		* 
		* @return the color value
		* 
		* @see get specified color
		*/
		const FLinearColor& get_color(const FWidgetStyle& in_widget_style) const
		{
			switch (m_color_use_rule)
			{
			case ESlateColorStylingMode::use_color_specified:
				return m_specified_color;
				break;
			case ESlateColorStylingMode::use_color_color_table://use theme color
				return get_color_from_table();//interms color table id from theme manager to get color
				break;
			case ESlateColorStylingMode::use_color_foreground:
				return in_widget_style.get_foreground_color();//use widget style's foreground color
				break;
			case ESlateColorStylingMode::use_color_foreground_subdued:
				return in_widget_style.get_subdued_foreground_color();//subdued foreground color
				break;
			default:
				break;
			}
		}

		const FLinearColor& get_color_from_table() const;
	protected:
		//glm::vec4 m_specified_color;

		//the current specified color, only meaningful when ColorToUse == UseColor_Specified
		FLinearColor m_specified_color;

		ESlateColorStylingMode m_color_use_rule;
	private:
		//id to a color in a color table to be used with ESlateColorStylingMode::use_color_color_table
		EStyleColor m_color_table_id;
	};
}