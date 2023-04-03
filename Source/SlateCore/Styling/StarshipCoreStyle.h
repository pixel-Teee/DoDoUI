#pragma once

#include "Core/Core.h"

#include "SlateCore/Layout/Margin.h"

#include "SlateCore/Fonts/SlateFontInfo.h"

#include "SlateCore/Fonts/CompositeFont.h"//FCompositeFont

namespace DoDo {
	class ISlateStyle;
	/*
	* core slate style
	*/
	class FStarshipCoreStyle
	{
	public:

		static std::shared_ptr<ISlateStyle> Create();

		/*get a font style using the default for slate*/
		static FSlateFontInfo get_default_font_style(const DoDoUtf8String in_type_face_font_name, const int32_t in_size, const FFontOutlineSettings& in_outline_settings = FFontOutlineSettings());

		static std::shared_ptr<const FCompositeFont> get_default_font();

		static void reset_to_default();//note:this function will call create

		static void set_style(const std::shared_ptr<ISlateStyle>& new_style);

		static const ISlateStyle& get_core_style();
	private:
		class FStyle;

		static void set_up_colors(std::shared_ptr<FStyle>& style);
		static void set_up_text_styles(std::shared_ptr<FStyle>& style);
		static void set_up_button_styles(std::shared_ptr<FStyle>& style);
		static void set_up_docking_styles(std::shared_ptr<FStyle>& style);

		/*singleton instances of this style*/
		static std::shared_ptr<ISlateStyle> m_instances;
	};

	struct FStyleFonts
	{
	public:
		static const FStyleFonts& get()
		{
			if (m_instance == nullptr)
			{
				m_instance = std::make_unique<FStyleFonts>();
			}
			return *m_instance;
		}

		const FSlateFontInfo m_normal;

		FStyleFonts();

		static std::unique_ptr<struct FStyleFonts> m_instance;
	};
}