#pragma once

#include "Core/Core.h"

#include "glm/glm.hpp"

namespace DoDo {
	enum class ESlateColorStylingMode : uint8_t
	{
		use_color_specified,

		use_color_color_table,

		use_color_foreground,//use the widget's foreground color

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
			: m_specified_color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
			, m_color_use_rule(ESlateColorStylingMode::use_color_specified)
		{}

		FSlateColor(const glm::vec4& in_color)
			: m_specified_color(in_color)
			, m_color_use_rule(ESlateColorStylingMode::use_color_specified)
		{}

		FSlateColor(EStyleColor in_color_table_id)
			: m_specified_color()
			, m_color_use_rule(ESlateColorStylingMode::use_color_color_table)
			, m_color_table_id(in_color_table_id)
		{}

		~FSlateColor()
		{}

	protected:
		glm::vec4 m_specified_color;

		ESlateColorStylingMode m_color_use_rule;
	private:
		//id to a color in a color table to be used with ESlateColorStylingMode::use_color_color_table
		EStyleColor m_color_table_id;
	};
}