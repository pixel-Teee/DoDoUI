#include <PreCompileHeader.h>

#include "SlateTypes.h"

#include "SlateCore/Styling/SlateDefaults.h"

#include "SlateCore/Brushes/SlateNoResource.h"

namespace DoDo
{
	const DoDoUtf8String FButtonStyle::TypeName("FButtonStyle");
	FButtonStyle::FButtonStyle()
	{
	}

	FButtonStyle::~FButtonStyle()
	{
	}

	const DoDoUtf8String FSplitterStyle::TypeName("FSplitterStyle");
	FSplitterStyle::FSplitterStyle()
	{
	}

	FSplitterStyle::~FSplitterStyle()
	{
	}

	void FSplitterStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		//FSlateWidgetStyle::get_resources(out_brushes);
		out_brushes.push_back(&m_handled_normal_brush);
		out_brushes.push_back(&m_handle_highlight_brush);
	}

	FDockTabStyle::FDockTabStyle()
		: m_icon_size(16, 16)//todo:fix me, use 16 x 16
		, m_overlap_width(0.0f)
		, m_normal_foreground_color(FSlateColor::use_foreground())
		, m_hovered_foreground_color(FSlateColor::use_foreground())
		, m_active_foreground_color(FSlateColor::use_foreground())
		, m_foreground_foreground_color(FSlateColor::use_foreground())
		, m_icon_border_padding(1.0f)
	{
	}

	FDockTabStyle::~FDockTabStyle()
	{
	}

	void FDockTabStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		m_close_button_style.get_resources(out_brushes);

		out_brushes.push_back(&m_normal_brush);
		out_brushes.push_back(&m_color_overlay_tab_brush);
		out_brushes.push_back(&m_color_overlay_icon_brush);
		out_brushes.push_back(&m_foreground_brush);
		out_brushes.push_back(&m_hovered_brush);
		out_brushes.push_back(&m_content_area_brush);
		out_brushes.push_back(&m_tab_well_brush);
	}

	const DoDoUtf8String FDockTabStyle::TypeName("FDockTabStyle");

	const FDockTabStyle& FDockTabStyle::get_default()
	{
		static FDockTabStyle Default;
		return Default;
	}

	FTextBlockStyle::FTextBlockStyle()
		: m_overflow_policy(ETextOverflowPolicy::Clip)
	{
	}

	const FTextBlockStyle& FTextBlockStyle::get_default()
	{
		static std::shared_ptr<FTextBlockStyle> Default;

		if (!Default)
		{
			Default = std::make_shared<FTextBlockStyle>();
			Default->m_font = FStyleDefaults::get_font_info();
		}

		return *Default.get();
	}

	void FTextBlockStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
	}

	const DoDoUtf8String FTextBlockStyle::TypeName("FTextBlockStyle");

	FSliderStyle::FSliderStyle()
		: m_bar_thick_ness(2.0f)
	{

	}

	void FSliderStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		out_brushes.push_back(&m_normal_bar_image);
		out_brushes.push_back(&m_hovered_bar_image);
		out_brushes.push_back(&m_disabled_bar_image);
		out_brushes.push_back(&m_disabled_thumb_image);
		out_brushes.push_back(&m_hovered_thumb_image);
		out_brushes.push_back(&m_disabled_thumb_image);
	}

	const DoDoUtf8String FSliderStyle::TypeName("FSliderStyle");

	const FSliderStyle& FSliderStyle::get_default()
	{
		static FSliderStyle Default;
		return Default;
	}

	FWindowStyle::FWindowStyle()
		: m_background_color(FLinearColor::White)
		, m_outline_color(FLinearColor::White)
		, m_border_color(FLinearColor::White)
		, m_window_corner_radius(0.0f)
		, m_broder_padding(FMargin(5.0f, 5.0f, 5.0f, 5.0f))
	{
	}

	const DoDoUtf8String FWindowStyle::TypeName("FWindowStyle");

	void FWindowStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		m_minimize_button_style.get_resources(out_brushes);
		m_maxmize_button_style.get_resources(out_brushes);
		m_restore_button_style.get_resources(out_brushes);
		m_close_button_style.get_resources(out_brushes);

		m_title_text_style.get_resources(out_brushes);

		out_brushes.push_back(&m_active_title_brush);
		out_brushes.push_back(&m_inactive_title_brush);
		out_brushes.push_back(&m_flash_title_brush);
		out_brushes.push_back(&m_border_brush);
		out_brushes.push_back(&m_background_brush);
		out_brushes.push_back(&m_child_background_brush);
	}

	const FWindowStyle& FWindowStyle::get_default()
	{
		static FWindowStyle Default;

		return Default;
	}

	FEditableTextBoxStyle::FEditableTextBoxStyle()
		: m_background_image_normal()
		, m_background_image_hovered()
		, m_background_image_focused()
		, m_background_image_read_only()
		, m_padding(FMargin(4.0f, 2.0f))
		, m_font(FStyleDefaults::get_font_info(9))
		, m_foreground_color(FSlateColor::use_foreground())
		, m_background_color(FLinearColor::White)
		, m_readonly_foreground_color(FSlateColor::use_foreground())
		, m_focused_foreground_color(FSlateColor::use_foreground())
	{
	}

	FEditableTextBoxStyle::~FEditableTextBoxStyle()
	{
	}

	const DoDoUtf8String FEditableTextBoxStyle::TypeName("FEditableTextBoxStyle");

	void FEditableTextBoxStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		out_brushes.push_back(&m_background_image_normal);
		out_brushes.push_back(&m_background_image_hovered);
		out_brushes.push_back(&m_background_image_focused);
		out_brushes.push_back(&m_background_image_read_only);

		//todo:add scroll bar style
	}

	const FEditableTextBoxStyle& FEditableTextBoxStyle::get_default()
	{
		static FEditableTextBoxStyle Default;
		return Default;
	}

	const DoDoUtf8String FScrollBarStyle::TypeName("FScrollBarStyle");

	FScrollBarStyle::FScrollBarStyle()
		: m_horizontal_background_image(FSlateNoResource())
		, m_vertical_background_image(FSlateNoResource())
		, m_vertical_top_slot_image(FSlateNoResource())
		, m_horizontal_top_slot_image(FSlateNoResource())
		, m_vertical_bottom_slot_image(FSlateNoResource())
		, m_horizontal_bottom_slot_image(FSlateNoResource())
		, m_normal_thumb_image(FSlateNoResource())
		, m_hovered_thumb_image(FSlateNoResource())
		, m_dragged_thumb_image(FSlateNoResource())
		, m_thickness(16.0f)
	{
	}

	void FScrollBarStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		out_brushes.push_back(&m_horizontal_background_image);
		out_brushes.push_back(&m_vertical_background_image);
		out_brushes.push_back(&m_vertical_top_slot_image);
		out_brushes.push_back(&m_horizontal_top_slot_image);
		out_brushes.push_back(&m_vertical_bottom_slot_image);
		out_brushes.push_back(&m_horizontal_bottom_slot_image);
		out_brushes.push_back(&m_normal_thumb_image);
		out_brushes.push_back(&m_hovered_thumb_image);
		out_brushes.push_back(&m_dragged_thumb_image);
	}

	const FScrollBarStyle& FScrollBarStyle::get_default()
	{
		static FScrollBarStyle Default;
		return Default;
	}

	//const FSplitterStyle& FSplitterStyle::get_default()
	//{
	//	static FSplitterStyle default;
	//	return default;
	//}
}
