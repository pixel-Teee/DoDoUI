#pragma once

#include "Core/Core.h"

#include "SlateColor.h"

namespace DoDo {
	enum class EStyleColor : uint8_t
	{
		Foreground,
		ForegroundHover
	};
	/*
	* common/themeable colors used by all styles
	*/
	struct FStyleColors
	{
		static const FSlateColor Foreground;//table id
		static const FSlateColor ForegroundHover;
		static const FSlateColor Primary;
		static const FSlateColor Input;
		static const FSlateColor Recessed;
		static const FSlateColor PrimaryHover;
		static const FSlateColor PrimaryPress;
		static const FSlateColor DropDown;
	};
}