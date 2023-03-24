#pragma once

#include "Core/Core.h"

#include "SlateColor.h"

#include "Core/String/DoDoString.h"

namespace DoDo {
	enum class EStyleColor : uint8_t
	{
		Foreground,
		ForegroundHover,
		Panel,
		Primary,
		Input,
		Recessed,
		PrimaryHover,
		PrimaryPress,
		DropDown,
		Background,
		Secondary,
		Hover,

		max
	};
	/*represents a single theme*/
	struct FStyleTheme
	{

	};

	struct FStyleColorList
	{
		FLinearColor m_style_colors[(int32_t)EStyleColor::max] = { FLinearColor::Transparent };

		DoDoUtf8String m_display_names[(int32_t)EStyleColor::max];
	};

	class USlateThemeManager //note:this class depend on UObject
	{
	public:
		static USlateThemeManager& get()
		{
			return m_singleton;
		}

		const FLinearColor& get_color(EStyleColor color)//note:pass a color table id to get specific theme FLinear Color
		{
			return m_active_colors.m_style_colors[static_cast<int32_t>(color)];
		}

		USlateThemeManager();

	private:
		static USlateThemeManager m_singleton;//default theme

		FStyleColorList m_active_colors;//FLinearColor list
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
		static const FSlateColor Panel;
		static const FSlateColor Background;
		static const FSlateColor Secondary;
		static const FSlateColor Hover;
	};
}