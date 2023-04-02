#pragma once

#include "Core/Core.h"

#include "SlateColor.h"

#include "Core/String/DoDoString.h"

#include "Core/Misc/Guid.h"//FGuid depends on it

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
		White,
		WindowBorder,
		InputOutline,
		Header,

		max
	};

#define get_enum_str(ENUM) #ENUM

	static const char* style_color_enum_str[(int32_t)EStyleColor::max] = {
		get_enum_str(EStyleColor::Foreground),
		get_enum_str(EStyleColor::ForegroundHover),
		get_enum_str(EStyleColor::Panel),
		get_enum_str(EStyleColor::Primary),
		get_enum_str(EStyleColor::Input),
		get_enum_str(EStyleColor::Recessed),
		get_enum_str(EStyleColor::PrimaryHover),
		get_enum_str(EStyleColor::PrimaryPress),
		get_enum_str(EStyleColor::DropDown),
		get_enum_str(EStyleColor::Background),
		get_enum_str(EStyleColor::Secondary),
		get_enum_str(EStyleColor::Hover),
		get_enum_str(EStyleColor::White),
		get_enum_str(EStyleColor::WindowBorder),
		get_enum_str(EStyleColor::InputOutline),
		get_enum_str(EStyleColor::Header)
	};

	/*represents a single theme*/
	struct FStyleTheme
	{
		/*unique id for the theme*/
		FGuid m_id;
		/*friendly, user customizable theme name*/
		DoDoUtf8String m_display_name;

		/*filename where the theme is stored*/
		DoDoUtf8String m_file_name;

		/*the default colors for this theme, used for resetting to default, not the active colors*/
		std::vector<FLinearColor> m_loaded_default_colors;
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

		/*
		* load all known themes from engine, project, and user directories
		*/
		void load_themes();

		const FLinearColor& get_color(EStyleColor color)//note:pass a color table id to get specific theme FLinear Color
		{
			return m_active_colors.m_style_colors[static_cast<int32_t>(color)];
		}

		USlateThemeManager();

		/*
		* @return the engine theme dir, engine themes are project agnostic
		*/
		DoDoUtf8String get_engine_theme_dir() const;

	private:
		FStyleTheme& get_mutable_current_theme()
		{
			for (uint32_t i = 0; i < m_themes.size(); ++i)
			{
				if (m_themes[i].m_id == m_current_theme_id)
				{
					return m_themes[i];
				}
			}
			//todo:check
		}

		void ensure_valid_current_theme();

		void load_theme_colors(FStyleTheme& theme);

		void load_themes_from_directory(const DoDoUtf8String& directory);

		bool read_theme(const std::string& theme_data, FStyleTheme& out_theme);

		/*
		* applies a themes as the active theme
		*/
		void apply_theme(FGuid theme_id);

		std::vector<FStyleTheme> m_themes;

		FGuid m_current_theme_id;

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
		static const FSlateColor Panel;
		static const FSlateColor Primary;
		static const FSlateColor Input;
		static const FSlateColor Recessed;
		static const FSlateColor PrimaryHover;
		static const FSlateColor PrimaryPress;
		static const FSlateColor DropDown;
		static const FSlateColor Background;
		static const FSlateColor Secondary;
		static const FSlateColor Hover;
		static const FSlateColor White;
		static const FSlateColor WindowBorder;
		static const FSlateColor InputOutline;
		static const FSlateColor Header;
	};
}