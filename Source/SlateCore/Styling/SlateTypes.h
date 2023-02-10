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
}
