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

#include "SlateCore/Brushes/SlateNoResource.h"//FSlateNoResource depends on it

#include "SlateCore/Brushes/SlateBoxBrush.h"//FSlateBoxBrush depends on it

#include "SlateCore/Brushes/SlateColorBrush.h"//FSlateColorBrush depends on it

namespace DoDo {
	using namespace CoreStyleConstants;

	std::shared_ptr<ISlateStyle> FStarshipCoreStyle::m_instances = nullptr;

	std::unique_ptr<struct FStyleFonts> FStyleFonts::m_instance = nullptr;

#define FONT(...) FSlateFontInfo(FCoreStyle::get_default_font(), __VA_ARGS__)

	FStyleFonts::FStyleFonts()
		: m_normal(FONT(10, "Regular"))//note:from the legacy slate font cache to get the font
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

		set_up_colors(style);

		const FStyleFonts& style_fonts = FStyleFonts::get();

		set_up_text_styles(style);

		const FTextBlockStyle& normal_text = style->get_widget_style<FTextBlockStyle>("NormalText");

		//important colors
		{
			style->set("DefaultForeground", default_fore_ground);
		}

		//SToolTip defaults
		{
			style->set("ToolTip.Font", get_default_font_style("Regular", 9));
			style->set("ToolTip.Background", new FSlateRoundedBoxBrush(FStyleColors::DropDown, 0.0f, FStyleColors::DropDownOutline, 1.0f));
			style->set("ToolTip.LargerFont", style_fonts.m_normal);
			style->set("ToolTip.BrightBackground", new FSlateBoxBrush("Old/ToolTip_BrightBackground", FMargin(8.0f / 64.0f)));
		}

		//SBorder defaults
		{
			style->set("Border", new FSlateColorBrush(FStyleColors::Panel));
		}

		//SSlider defaults
		{
			FSliderStyle slider_style = FSliderStyle()
				.set_normal_bar_image(FSlateRoundedBoxBrush(FStyleColors::Input, 2.0f, FStyleColors::Input, 1.0f))
				.set_hovered_bar_image(FSlateRoundedBoxBrush(FStyleColors::Input, 2.0f, FStyleColors::Input, 1.0f))
				.set_normal_thumb_image(FSlateRoundedBoxBrush(FStyleColors::Foreground, glm::vec2(8.0f, 8.0f)))
				.set_hovered_thumb_image(FSlateRoundedBoxBrush(FStyleColors::ForegroundHover, glm::vec2(8.0f, 8.0f)))
				.set_bar_thickness(20.0f);

			style->set("Slider", slider_style);
		}

		//common brushes
		
		{
			style->set("Checkboard", new FSlateImageBrush(style->root_to_content_dir("Checkboard", ".png"), Icon16x16, FLinearColor::White, ESlateBrushTileType::Both));
		}

		//SSplitter
		{
			style->set("Splitter", FSplitterStyle()
				.set_handle_normal_brush(FSlateColorBrush(FStyleColors::Background))
				.set_handle_highlight_brush(FSlateColorBrush(FStyleColors::Secondary))
			);
		}

		//common icons
		{
			//chrome icon or not (we are testing this)
			style->set("Icons.ArrowLeft", new FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/arrow-left", ".svg"), Icon16x16));
			style->set("Icons.C++", new FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/CPP", ".svg"), Icon100x100));
		}

		//heart
		{
			style->set("Icons.heart2", new FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/heart2", ".svg"), Icon100x100));
		}

		//solar-system
		{
			style->set("Icons.solar-system", new FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/solar-system", ".svg"), Icon100x100));
		}

		//SColorWheel defaults
		{
			style->set("ColorWheel.HueValueCircle", new FSlateImageBrush(style->root_to_content_dir("Common/ColorWheel", ".png"), glm::vec2(192.0f, 192.0f)));
			style->set("ColorWheel.Selector", new FSlateImageBrush(style->root_to_content_dir("Common/Circle", ".png"), glm::vec2(8.0f, 8.0f)));
		}

		//tree-decidious-svgrepo-com
		{
			style->set("Icons.tree-decidious-svgrepo-com", new FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/tree-decidious-svgrepo-com", ".svg"), Icon16x16));
		}

		//invisible buttons, borders, etc.
		const FButtonStyle No_Border = FButtonStyle()
			.set_normal(FSlateNoResource())
			.set_hovered(FSlateNoResource())
			.set_pressed(FSlateNoResource())
			.set_normal_foreground(FStyleColors::ForegroundHover)
			.set_hovered_foreground(FStyleColors::ForegroundHover)
			.set_pressed_foreground(FStyleColors::ForegroundHover)
			.set_normal_padding(FMargin(0.0f))
			.set_pressed_padding(FMargin(0.0f));

		//convenient transprent/invsible elements
		{
			style->set("NoBrush", new FSlateNoResource());

			style->set("NoBorder", No_Border);
		}

		//appicon image
		{
			style->set("AppIcon", new FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/gamepad-svgrepo-com", ".svg"), glm::vec2(36.0f, 36.0f)));
		}

		//SWindow defaults
		{
			const FButtonStyle minimize_button_style = FButtonStyle(No_Border)
				.set_normal(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/minimize", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::Foreground))
				.set_hovered(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/minimize", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover))
				.set_pressed(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/minimize", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover));

			const FButtonStyle maximize_button_style = FButtonStyle(No_Border)
				.set_normal(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/maximize", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::Foreground))
				.set_hovered(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/maximize", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover))
				.set_pressed(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/maximize", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover));

			const FButtonStyle restore_button_style = FButtonStyle(No_Border)
				.set_normal(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/restore", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::Foreground))
				.set_hovered(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/restore", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover))
				.set_pressed(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/restore", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover));

			const FButtonStyle close_button_style = FButtonStyle(No_Border)
				.set_normal(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/close", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::Foreground))
				.set_hovered(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/close", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover))
				.set_pressed(FSlateVectorImageBrush(style->root_to_content_dir("Starship/CoreWidgets/Window/close", ".svg"), glm::vec2(42.0f, 34.0f), FStyleColors::ForegroundHover));

			FWindowStyle window =
				FWindowStyle()
				.set_minimize_button_style(minimize_button_style)
				.set_maximize_button_style(maximize_button_style)
				.set_restore_button_style(restore_button_style)
				.set_close_button_style(close_button_style)
				.set_title_text_style(normal_text)
				.set_active_title_brush(FSlateNoResource())
				.set_inactive_title_brush(FSlateNoResource())
				.set_flash_title_brush(FSlateImageBrush(style->root_to_content_dir("Common/Window/WindowTitle_Flashing", ".png"), Icon24x24, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::Horizontal))
				.set_background_brush(FSlateColorBrush(FStyleColors::Recessed))
				.set_outline_brush(FSlateRoundedBoxBrush(FStyleColors::Recessed, 2.0f, FStyleColors::WindowBorder, 2.0f))
				.set_child_background_brush(FSlateRoundedBoxBrush(FStyleColors::Recessed, 2.0f, FStyleColors::InputOutline, 2.0f))
				.set_corner_radius(2.0f)
				.set_border_padding(FMargin(3.0f, 3.0f, 3.0f, 3.0f));
			
			style->set("Window", window);

			window.set_corner_radius(8.0f);
		}
		
		set_up_button_styles(style);
		set_up_docking_styles(style);

		set_up_color_picker_styles(style);

		return style;
	}

	std::shared_ptr<const FCompositeFont> FStarshipCoreStyle::get_default_font()
	{
		return FCoreStyle::get_default_font();
	}

	FSlateFontInfo FStarshipCoreStyle::get_default_font_style(const DoDoUtf8String in_type_face_font_name, const int32_t in_size, const FFontOutlineSettings& in_outline_settings)
	{
		return FSlateFontInfo(get_default_font(), in_size, in_type_face_font_name, in_outline_settings);
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

	void FStarshipCoreStyle::set_up_colors(std::shared_ptr<FStyle>& style)
	{
		style->set("Brushes.Background", new FSlateColorBrush(FStyleColors::Background));
		style->set("Brushes.Panel", new FSlateColorBrush(FStyleColors::Panel));
	}

	void FStarshipCoreStyle::set_up_text_styles(std::shared_ptr<FStyle>& style)
	{
		const FStyleFonts& style_fonts = FStyleFonts::get();

		const FTextBlockStyle normal_text = FTextBlockStyle()
			.set_font(style_fonts.m_normal)
			.set_color_and_opacity(FSlateColor::use_foreground());

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
			.set_normal(FSlateRoundedBoxBrush(FStyleColors::Primary, 4.0f, FStyleColors::Input, input_focus_thickness))//FSlateRoundedBoxBrush
			.set_hovered(FSlateRoundedBoxBrush(FStyleColors::PrimaryHover, 4.0f, FStyleColors::Input, input_focus_thickness))
			.set_pressed(FSlateRoundedBoxBrush(FStyleColors::PrimaryPress, 4.0f, FStyleColors::Input, input_focus_thickness))
			.set_disabled(FSlateRoundedBoxBrush(FStyleColors::DropDown, 4.0f, FStyleColors::Recessed, input_focus_thickness))
			.set_normal_foreground(FStyleColors::ForegroundHover)
			.set_pressed_foreground(FStyleColors::ForegroundHover)
			.set_hovered_foreground(FStyleColors::ForegroundHover)
			.set_disabled_foreground(FStyleColors::Foreground)//todo:fix me
			//.set_normal_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			//.set_pressed_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			//.set_hovered_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			//.set_disabled_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))//todo:fix me
			.set_normal_padding(button_margins)
			.set_pressed_padding(pressed_button_margins);

		style->set("PrimaryButton", primary_button);//register this 

		const FButtonStyle Button = FButtonStyle()
			.set_normal(FSlateRoundedBoxBrush(FStyleColors::Secondary, 4.0f, FStyleColors::Input, input_focus_thickness))//FSlateRoundedBoxBrush
			.set_hovered(FSlateRoundedBoxBrush(FStyleColors::Hover, 4.0f, FStyleColors::Input, input_focus_thickness))
			.set_pressed(FSlateRoundedBoxBrush(FStyleColors::Header, 4.0f, FStyleColors::Input, input_focus_thickness))
			.set_disabled(FSlateRoundedBoxBrush(FStyleColors::DropDown, 4.0f, FStyleColors::Recessed, input_focus_thickness))
			.set_normal_foreground(FStyleColors::ForegroundHover)
			.set_pressed_foreground(FStyleColors::ForegroundHover)
			.set_hovered_foreground(FStyleColors::ForegroundHover)
			.set_disabled_foreground(FStyleColors::Foreground)//todo:fix me
			//.set_normal_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			//.set_pressed_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			//.set_hovered_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			//.set_disabled_foreground(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))//todo:fix me
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

		const FTextBlockStyle& normal_text = style->get_widget_style<FTextBlockStyle>("NormalText");

		const FButtonStyle& button = style->get_widget_style<FButtonStyle>("Button");

		//todo:fix me, use close small
		const FButtonStyle close_button = FButtonStyle()
			.set_normal(FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/close-small", ".svg"), Icon16x16, FStyleColors::Foreground))
			.set_pressed(FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/close-small", ".svg"), Icon16x16, FStyleColors::Foreground))
			.set_hovered(FSlateVectorImageBrush(style->root_to_content_dir("Starship/Common/close-small", ".svg"), Icon16x16, FStyleColors::ForegroundHover));
				

		FDockTabStyle minor_tab_style =
			FDockTabStyle()
			.set_close_button_style(close_button)
			.set_normal_brush(FSlateNoResource())
			.set_hovered_brush(FSlateBoxBrush(style->root_to_content_dir("Starship/Docking/DockTab_Foreground", ".png"), 4.0f / 20.0f, FStyleColors::Panel.get_specified_color().copy_with_new_opacity(0.8)))
			.set_foreground_brush(FSlateBoxBrush(style->root_to_content_dir("Starship/Docking/DockTab_Foreground", ".png"), 4.0f / 20.0f, FStyleColors::Foreground))
			.set_color_overlay_tab_brush(FSlateNoResource())
			.set_color_overlay_icon_brush(FSlateNoResource())
			.set_content_area_brush(FSlateColorBrush(FStyleColors::Panel))
			.set_tab_well_brush(FSlateColorBrush(FStyleColors::Background))
			//.set_flash_color()//todo:add tab flash color

			.set_tab_padding(FMargin(4.0f, 3.0f, 10.0f, 4.0f))
			.set_overlap_width(-2.0f) //negative overlap width to add space between tabs

			.set_normal_foreground_color(FStyleColors::Foreground)
			.set_active_foreground_color(FStyleColors::ForegroundHover)
			.set_foreground_foreground_color(FStyleColors::Foreground)
			.set_hovered_foreground_color(FStyleColors::ForegroundHover)
			.set_tab_text_style(normal_text);

		style->set("Docking.UnhideTabwellButton", FButtonStyle(button)
			.set_normal(FSlateImageBrush(style->root_to_content_dir("Docking/ShowTabwellButton_Normal", ".png"), glm::vec2(100.0f, 100.0f)))
			.set_pressed(FSlateImageBrush(style->root_to_content_dir("Docking/ShowTabwellButton_Pressed", ".png"), glm::vec2(100.0f, 100.0f)))
			.set_hovered(FSlateImageBrush(style->root_to_content_dir("Docking/ShowTabwellButton_Hovered", ".png"), glm::vec2(100.0f, 100.0f)))
			.set_normal_padding(0)
			.set_pressed_padding(0));

		//panel tab
		style->set("Docking.Tab", minor_tab_style);

		//app tab
		style->set("Docking.MajorTab", FDockTabStyle()
			.set_close_button_style(close_button)
			.set_normal_brush(FSlateNoResource())
			.set_hovered_brush(FSlateBoxBrush(style->root_to_content_dir("Starship/Docking/DockTab_Foreground", ".png"), 4.0f / 20.0f, FStyleColors::Foreground)) //panel + 20% block
			.set_foreground_brush(FSlateBoxBrush(style->root_to_content_dir("Starship/Docking/DockTab_Foreground", ".png"), 4.0f / 20.0f, FStyleColors::Panel))
			.set_color_overlay_tab_brush(FSlateNoResource())
			.set_color_overlay_icon_brush(FSlateRoundedBoxBrush(FStyleColors::Foreground, 4.0f))
			.set_content_area_brush(FSlateColorBrush(FStyleColors::Background))
			.set_tab_well_brush(FSlateColorBrush(FStyleColors::Background))

			.set_tab_padding(FMargin(4.0f, 4.0f, 10.0f, 4.0f))
			.set_overlap_width(-2.0f)//negative overlap width to ad space between tabs
			//todo:add tab flash color

			.set_normal_foreground_color(FStyleColors::Foreground)
			.set_active_foreground_color(FStyleColors::White)
			.set_foreground_foreground_color(FStyleColors::White)
			.set_hovered_foreground_color(FStyleColors::White)
			.set_tab_text_style(normal_text)
			.set_icon_border_padding(4.0f)
		);

		//todo:add more tab
		style->set("Docking.MajorTab", minor_tab_style);

		//dock cross
		style->set("Docking.Cross.DockLeft", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.25f)));
		style->set("Docking.Cross.DockLeft_Hovered", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.0f)));

		style->set("Docking.Cross.DockTop", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.25f)));
		style->set("Docking.Cross.DockTop_Hovered", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.0f)));

		style->set("Docking.Cross.DockRight", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.25f)));
		style->set("Docking.Cross.DockRight_Hovered", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.0f)));

		style->set("Docking.Cross.DockBottom", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.25f)));
		style->set("Docking.Cross.DockBottom_Hovered", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.0f)));

		style->set("Docking.Cross.DockCenter", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator_Center", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.25f)));
		style->set("Docking.Cross.DockCenter_Hovered", new FSlateImageBrush(style->root_to_content_dir("Docking/OuterDockingIndicator_Center", ".png"), glm::vec2(6.0f, 6.0f), FLinearColor(1.0f, 0.35f, 0.0f, 0.0f)));
	
		style->set("Docking.Cross.BorderLeft", new FSlateNoResource());
		style->set("Docking.Cross.BorderTop", new FSlateNoResource());
		style->set("Docking.Cross.BorderRight", new FSlateNoResource());
		style->set("Docking.Cross.BorderBottom", new FSlateNoResource());
		style->set("Docking.Cross.BorderCenter", new FSlateNoResource());

		style->set("Docking.Cross.Tint", FLinearColor::White);
	}

	void FStarshipCoreStyle::set_up_multibox_styles(std::shared_ptr<FStyle>& style)
	{
		//menu bar
		{
			style->set("Menu.Background", new FSlateColorBrush(FStyleColors::DropDown));
			style->set("Menu.Outline", new FSlateRoundedBoxBrush(FStyleColors::Transparent, 0.0f, FStyleColors::DropDownOutline, 1.0f));
		}
	}

	void FStarshipCoreStyle::set_up_color_picker_styles(std::shared_ptr<FStyle>& style)
	{
		//SColorPicker defaults
		{
			style->set("ColorPicker.Slider", FSliderStyle()
				.set_normal_thumb_image(FSlateImageBrush(style->root_to_content_dir("Common/ColorPicker_SliderHandle", ".png"), glm::vec2(8.0f, 32.0f)))
				.set_normal_thumb_image(FSlateImageBrush(style->root_to_content_dir("Common/ColorPicker_SliderHandle", ".png"), glm::vec2(8.0f, 32.0f)))
				.set_hovered_thumb_image(FSlateImageBrush(style->root_to_content_dir("Common/ColorPicker_SliderHandle", ".png"), glm::vec2(8.0f, 32.0f))));
		}
	}

}