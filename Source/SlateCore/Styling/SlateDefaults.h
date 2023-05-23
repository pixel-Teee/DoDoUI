#pragma once

#include "SlateCore/Styling/SlateColor.h"//FLinearColor depends on it

#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo

namespace DoDo {
	struct FSlateBrush;
	class FStyleDefaults
	{
	public:
		/*
		* @return - returns no brush
		*/
		static const FSlateBrush* get_no_brush();

		/*
		* get default FLinearColor
		* 
		* @return - the default FLinearColor
		*/
		static const FLinearColor& get_color();

		static const FSlateFontInfo get_font_info(uint16_t size = 10);

	private:

		static FLinearColor m_default_color;
	};
}