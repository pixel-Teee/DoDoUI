#pragma once

#include "SlateWidgetStyle.h"

#include "SlateBrush.h"

namespace DoDo
{
	/*represents the appearance of an SButton*/
	struct FButtonStyle : public FSlateWidgetStyle
	{
		FButtonStyle();

		virtual ~FButtonStyle();

		static const DoDoUtf8String TypeName;
		virtual const DoDoUtf8String get_type_name() const override { return TypeName; }

		/*button appearance when the button is not hovered or pressed*/
		FSlateBrush m_normal;
		FButtonStyle& set_normal(const FSlateBrush& in_normal) { m_normal = in_normal; return *this; }

		/*button appearance when hovered*/
		FSlateBrush m_hovered;
		FButtonStyle& set_hovered(const FSlateBrush& in_hovered) { m_hovered = in_hovered; return *this; }

		/*button appearance when pressed*/
		FSlateBrush m_pressed;
		FButtonStyle& set_pressed(const FSlateBrush& in_pressed) { m_pressed = in_pressed; return *this; }

		/*button appearance when disabled, by default this is set to an invalid resource when that is the case default disabled drawing is used*/
		FSlateBrush m_disabled;
		FButtonStyle& set_disabled(const FSlateBrush& in_disabled) { m_disabled = in_disabled; return *this; }

		/*foreground color when the button is not hovered or pressed*/
		glm::vec4 m_normal_foreground;
		FButtonStyle& set_normal_foreground(const glm::vec4& in_normal_foreground) { m_normal_foreground = in_normal_foreground; return *this; }

		/*foreground color when hovered*/
		glm::vec4 m_hovered_foreground;
		FButtonStyle& set_hovered_foreground(const glm::vec4& in_hovered_foreground) { m_hovered_foreground = in_hovered_foreground; return *this; }

		/*foreground color when pressed*/
		glm::vec4 m_pressed_foreground;
		FButtonStyle& set_pressed_foreground(const glm::vec4& in_pressed_foreground) { m_pressed_foreground = in_pressed_foreground; return *this; }

		/*foreground color when disabled*/
		glm::vec4 m_disabled_foreground;
		FButtonStyle& set_disabled_foreground(const glm::vec4 in_disabled_foreground) { m_disabled_foreground = in_disabled_foreground; return *this; }

		/*
		 * padding that accounts for the border in the button's background image
		 * when this is applied, the content of the button should appear flush
		 * with the button's border, use this padding when the button is not pressed
		 */
		FMargin m_normal_padding;
		FButtonStyle& set_normal_padding(const FMargin& in_normal_padding) { m_normal_padding = in_normal_padding; return *this; }

		/*
		 * same as NormalPadding but used when the button is pressed, allows for moving the content to match
		 * any "movement" in the button's border image
		 */
		FMargin m_pressed_padding;
		FButtonStyle& set_pressed_padding(const FMargin& in_pressed_padding) { m_pressed_padding = in_pressed_padding; return *this; }

		//todo:implement unlink colors function
	};

	/*
	 * represents the appearance of an SSplitter
	 */
	struct FSplitterStyle : public FSlateWidgetStyle
	{
		FSplitterStyle();

		virtual ~FSplitterStyle();

		virtual void get_resources(std::vector<const FSlateBrush*>& out_brushes) const override;

		static const DoDoUtf8String TypeName;

		virtual const DoDoUtf8String get_type_name() const override { return TypeName; }

		//static const FSplitterStyle& get_default();

		/*brush used to draw the handle in it's normal state*/
		FSlateBrush m_handled_normal_brush;
		FSplitterStyle& set_handle_normal_brush(const FSlateBrush& in_handle_normal_brush) { m_handled_normal_brush = in_handle_normal_brush; return *this; }

		/*brush used to draw the handle in it's highlight state*/
		FSlateBrush m_handle_highlight_brush;
		FSplitterStyle& set_handle_highlight_brush(const FSlateBrush& in_handle_highlight_brush) { m_handle_highlight_brush = in_handle_highlight_brush; return *this; }
	};

	/*
	* represents the appearance of an SDockTab
	*/
	struct FDockTabStyle : public FSlateWidgetStyle
	{
		FDockTabStyle();

		virtual ~FDockTabStyle();

		virtual void get_resources(std::vector<const FSlateBrush*>& out_brushes) const override;

		static const DoDoUtf8String TypeName;

		virtual const DoDoUtf8String get_type_name() const override { return TypeName; }

		static const FDockTabStyle& get_default();

		/*style used for the close button*/
		FButtonStyle m_close_button_style;
		FDockTabStyle& set_close_button_style(const FButtonStyle& in_close_button_style) { m_close_button_style = in_close_button_style; return *this; }

		/*brush used when this tab is in its normal state*/
		FSlateBrush m_normal_brush;
		FDockTabStyle& set_normal_brush(const FSlateBrush& in_normal_brush) { m_normal_brush = in_normal_brush; return *this; }

		/*brush used to overlay a given color onto this tab*/
		FSlateBrush m_color_overlay_tab_brush;
		FDockTabStyle& set_color_overlay_tab_brush(const FSlateBrush& in_color_overlay_brush) { m_color_overlay_tab_brush = in_color_overlay_brush; return *this; }

		/*brush used to overlay a given color onto this tab*/
		FSlateBrush m_color_overlay_icon_brush;
		FDockTabStyle& set_color_overlay_icon_brush(const FSlateBrush& in_color_overlay_brush) { m_color_overlay_icon_brush = in_color_overlay_brush; return *this; }

		/*brush used to when this tab is in the foreground*/
		FSlateBrush m_foreground_brush;
		FDockTabStyle& set_foreground_brush(const FSlateBrush& in_foreground_brush) { m_foreground_brush = in_foreground_brush; return *this; }

		/*brush used when this tab is hovered over*/
		FSlateBrush m_hovered_brush;
		FDockTabStyle& set_hovered_brush(const FSlateBrush& in_hovered_brush) { m_hovered_brush = in_hovered_brush; return *this; }

		/*brush used by the SDockingTabStack to draw the content associated with this tab, documents, apps, and tool panels have different backgrounds*/
		FSlateBrush m_content_area_brush;
		FDockTabStyle& set_content_area_brush(const FSlateBrush& in_content_area_brush) { m_content_area_brush = in_content_area_brush; return *this; }

		/*brush used by the SDockingTabStack to draw the content associated with this tab, documents, apps, and tool panels have different backgrounds*/
		FSlateBrush m_tab_well_brush;
		FDockTabStyle& set_tab_well_brush(const FSlateBrush& in_tab_well_brush) { m_tab_well_brush = in_tab_well_brush; return *this; }

		//todo:implement tab text style

		/*padding used around this tab*/
		FMargin m_tab_padding;
		FDockTabStyle& set_tab_padding(const FMargin& in_tab_padding) { m_tab_padding = in_tab_padding; return *this; }

		/*icon size for icons in this tab*/
		glm::vec2 m_icon_size;
		FDockTabStyle& set_icon_size(const glm::vec2& in_icon_size) { m_icon_size = in_icon_size; return *this; }

		/*the width that this tab will overlap with side-by-side tabs*/
		float m_overlap_width;
		FDockTabStyle& set_overlap_width(const float in_overlap_width) { m_overlap_width = in_overlap_width; return *this; }

		/*color used when flashing this tab*/
		glm::vec4 m_flash_color;
		FDockTabStyle& set_flash_color(const glm::vec4& in_flash_color) { m_flash_color = in_flash_color; return *this; }

		/*foreground color when the tab is not hovered, pressed, active or in the foreground*/
		glm::vec4 m_normal_foreground_color;
		FDockTabStyle& set_normal_foreground_color(const glm::vec4& in_normal_foreground_color) { m_normal_foreground_color = in_normal_foreground_color; return *this; }

		/*foreground color when hovered*/
		glm::vec4 m_hovered_foreground_color;
		FDockTabStyle& set_hovered_foreground_color(const glm::vec4& in_hovered_foreground_color) { m_hovered_foreground_color = in_hovered_foreground_color; return *this; }

		/*foreground color when active*/
		glm::vec4 m_active_foreground_color;
		FDockTabStyle& set_active_foreground_color(const glm::vec4& in_active_foreground_color) { m_active_foreground_color = in_active_foreground_color; return *this; }

		/*foreground color when this tab is the foreground tab*/
		glm::vec4 m_foreground_foreground_color;
		FDockTabStyle& set_foreground_foreground_color(const glm::vec4& in_foreground_foreground_color) { m_foreground_foreground_color = in_foreground_foreground_color; return *this; }

		/*the padding applied to the border around the tab icon*/
		float m_icon_border_padding;
		FDockTabStyle& set_icon_border_padding(const float in_icon_border_padding) { m_icon_border_padding = in_icon_border_padding; return *this; }
	};
}
