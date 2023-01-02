#pragma once

#include "Slate/Widgets/Layout/SBorder.h"

namespace DoDo
{
	class SButton : public SBorder
	{
		SLATE_DECLARE_WIDGET(SButton, SBorder)

	public:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
	private:
		/*the location in screen space the button was pressed*/
		glm::vec2 m_pressed_screen_space_position;

		/*style resource for the button*/
		//todo:implement FButtonStyle

		/*the delegate to execute when button is clicked*/
		//todo:implement FReply

		/*the delegate to execute when button is pressed*/
		//todo:implement FSlateDelegate

		/*the delegate to execute when button is released*/
		//todo:implement

		/*the delegate to execute when button is hovered*/
		//todo:implement

		/*the delegate to execute when the button exit the hovered state*/
		//todo:implement

		/*the sound to play when the button is hovered*/
		//todo:implement FSlateSound

		/*sets whether a click should be triggered on mouse down, mouse up, or that both a mouse down and up are required*/
		//todo:implement ClickMethod

		//todo:implement TouchMethod

		//todo:implement PressMethod

		/*can this button be focused?*/
		uint8_t m_b_is_focusable : 1;

		/*true if this button is currently in a pressed state*/
		uint8_t m_b_is_pressed : 1;

	private:
		/*other color information*/
		//todo:implement
	};
}