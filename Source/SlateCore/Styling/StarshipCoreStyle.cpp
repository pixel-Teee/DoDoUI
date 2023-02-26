#include <PreCompileHeader.h>

#include "StarshipCoreStyle.h"

#include "SlateStyle.h"//FStartshipCoreStyle::FStyle depends on it

#include "glm/glm.hpp"

#include "Core/Misc/Paths.h"

#include "StyleColors.h"

#include "CoreStyle.h"

#include "SlateTypes.h"//FButtonStyle

#include "SlateCore/Brushes/SlateRoundedBoxBrush.h"

#include "SlateStyleRegistry.h"//central factory

#include "SlateCore/Brushes/SlateColorBrush.h"

#include "SlateCore/Brushes/SlateImageBrush.h"

#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo depends on it

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

namespace DoDo {
	using namespace CoreStyleConstants;

	std::shared_ptr<ISlateStyle> FStarshipCoreStyle::m_instances = nullptr;

	std::unique_ptr<struct FStyleFonts> FStyleFonts::m_instance = nullptr;

#define FONT(...) FSlateFontInfo(FCoreStyle::get_default_font(), __VA_ARGS__)

	FStyleFonts::FStyleFonts()
		: m_normal(FONT(50, "Regular"))//note:from the legacy slate font cache to get the font
	{
	}

	class FStarshipCoreStyle::FStyle : public FSlateStyleSet
	{
	public:
		FStyle(const DoDoUtf8String& in_style_set_name)
			: FSlateStyleSet(in_style_set_name)
			, m_selector_color_linear(std::make_shared<glm::vec4>(0.70f, 0.225f, 0.003f, 1.0f))
			, m_selection_color_linear(std::make_shared<glm::vec4>(0.70f, 0.225f, 0.003f, 1.0f))//todo:fix me
			, m_selection_color_inactive_linear(std::make_shared<glm::vec4>(0.25f, 0.25f, 0.25f, 1.0f))
			, m_selection_color_pressed_linear(std::make_shared<glm::vec4>(0.701f, 0.225f, 0.003f, 1.0f))
			, m_highlight_color_linear(std::make_shared<glm::vec4>(0.068f, 0.068f, 0.068f, 1.0f))
		{

		}

		static void set_color(const std::shared_ptr<glm::vec4>& source, const glm::vec4& value)
		{
			source->x = value.x;
			source->y = value.y;
			source->z = value.z;
			source->w = value.w;
		}

		//these are the colors that are updated by the user style customizations
		const std::shared_ptr<glm::vec4> m_selector_color_linear;

		const std::shared_ptr<glm::vec4> m_selection_color_linear;

		const std::shared_ptr<glm::vec4> m_selection_color_inactive_linear;

		const std::shared_ptr<glm::vec4> m_selection_color_pressed_linear;

		const std::shared_ptr<glm::vec4> m_highlight_color_linear;
	};

	std::shared_ptr<ISlateStyle> FStarshipCoreStyle::Create()
	{
		//todo:create many things
		std::shared_ptr<FStyle> style = std::make_shared<FStyle>("CoreStyle");

		style->set_content_root(FPaths::engine_dir() / "Slate");

		const DoDoUtf8String& canary_path = style->root_to_content_dir("Checkboard", ".png");//todo:interms of FPlatformMisc::EngineDir to get root directory
		//canary_path just to check

		if (!std::filesystem::exists(canary_path.c_str()))
		{
			//error
			return style;
		}


		//const std::string canary_path;
		const FSlateColor default_fore_ground(FStyleColors::Foreground);//use color table id

		const FStyleFonts& style_fonts = FStyleFonts::get();

		set_up_text_styles(style);

		//important colors
		{
			style->set("DefaultForeground", default_fore_ground);
		}

		//SBorder defaults
		{
			style->set("Border", new FSlateColorBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
		}

		//common brushes
		
		{
			style->set("Checkboard", new FSlateImageBrush(style->root_to_content_dir("Checkboard", ".png"), Icon16x16, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::Both));
		}

		//SSplitter
		{
			style->set("Splitter", FSplitterStyle()
				.set_handle_normal_brush(FSlateColorBrush(glm::vec4(0.7f, 0.3f, 0.2f, 1.0f)))
				.set_handle_highlight_brush(FSlateColorBrush(glm::vec4(0.9f, 0.3f, 0.2f, 1.0f)))
			);
		}

		set_up_button_styles(style);
		set_up_docking_styles(style);

		return style;
	}
	void FStarshipCoreStyle::reset_to_default()
	{
		//todo:implement set style
		//create a private class, this class inherited from ISlateStyle, set to instance
		set_style(FStarshipCoreStyle::Create());
	}

	void FStarshipCoreStyle::set_style(const std::shared_ptr<ISlateStyle>& new_style)
	{
		if (m_instances)//check valid
		{
			//todo:from FSlateStyleRegistry to unregister this
		}

		m_instances = new_style;

		if (m_instances)
		{
			//register to middle factory
			//register to central
			FSlateStyleRegistry::register_slate_style(*m_instances.get());
		}
	}

	const ISlateStyle& FStarshipCoreStyle::get_core_style()
	{
		return *m_instances;
	}

	void FStarshipCoreStyle::set_up_text_styles(std::shared_ptr<FStyle>& style)
	{
		const FStyleFonts& style_fonts = FStyleFonts::get();

		const FTextBlockStyle normal_text = FTextBlockStyle()
			.set_font(style_fonts.m_normal);

		style->set("NormalFont", style_fonts.m_normal);

		//STextBlock defaults...
		style->set("NormalText", normal_text);
	}

	void FStarshipCoreStyle::set_up_button_styles(std::shared_ptr<FStyle>& style)
	{
		//sbutton defaults
		const FButtonStyle primary_button = FButtonStyle()
			//.set_normal(FSlateRoundedBoxBrush(FStyleColors::Primary, 4.0f, FStyleColors::Input, input_focus_thickness))//FSlateRoundedBoxBrush
			//.set_hovered(FSlateRoundedBoxBrush(FStyleColors::Primary, 4.0f, FStyleColors::Input, input_focus_thickness))
			//.set_pressed(FSlateRoundedBoxBrush(FStyleColors::Primary, 4.0f, FStyleColors::Input, input_focus_thickness))
			//.set_disabled(FSlateRoundedBoxBrush(FStyleColors::Primary, 4.0f, FStyleColors::Recessed, input_focus_thickness))
			.set_normal(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))//FSlateRoundedBoxBrush
			.set_hovered(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))
			.set_pressed(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))
			.set_disabled(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))
			//.set_normal_foreground(FStyleColors::ForegroundHover)
			//.set_pressed_foreground(FStyleColors::ForegroundHover)
			//.set_hovered_foreground(FStyleColors::ForegroundHover)
			//.set_disabled_foreground(FStyleColors::Foreground)//todo:fix me
			.set_normal_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			.set_pressed_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			.set_hovered_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			.set_disabled_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))//todo:fix me
			.set_normal_padding(button_margins)
			.set_pressed_padding(pressed_button_margins);

		style->set("PrimaryButton", primary_button);//register this 

		const FButtonStyle Button = FButtonStyle()
			.set_normal(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))//FSlateRoundedBoxBrush
			.set_hovered(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))
			//se.set_pressed(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))t_up_button_styles(style);
			.set_disabled(FSlateRoundedBoxBrush(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), input_focus_thickness))
			.set_normal_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			.set_pressed_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			.set_hovered_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			.set_disabled_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))//todo:fix me
			.set_normal_padding(button_margins)
			.set_pressed_padding(pressed_button_margins);

		style->set("Button", Button);
	}
	void FStarshipCoreStyle::set_up_docking_styles(std::shared_ptr<FStyle>& style)
	{
		//todo:implement close button

		//FDockTabStyle minor_tab_style = 
		//	FDockTabStyle()
		//	.set_close_button_style()
	}

}