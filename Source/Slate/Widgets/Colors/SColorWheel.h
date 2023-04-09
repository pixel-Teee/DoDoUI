#pragma once

#include "SlateCore/Widgets/SLeafWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "Slate/Framework/SlateDelegates.h"

namespace DoDo {
	struct FSlateBrush;
	/*
	* implements the color wheel widget
	*/
	class SColorWheel : public SLeafWidget
	{
	public:
		SLATE_BEGIN_ARGS(SColorWheel)
			: _SelectedColor()
			, _OnMouseCaptureBegin()
		{}
			/*the current color selected by the user*/
			SLATE_ATTRIBUTE(FLinearColor, SelectedColor)

			/*invoked when the mouse is pressed and a capture begins*/
			SLATE_EVENT(FSimpleDelegate, OnMouseCaptureBegin)

			/*invoked when the mouse is released and a capture ends*/
			SLATE_EVENT(FSimpleDelegate, OnMouseCaptureEnd)

			/*invoked when a new value is selected on the color wheel*/
			SLATE_EVENT(FOnLinearColorValueChanged, OnValueChanged)

		SLATE_END_ARGS()
	public:
		SColorWheel();

		/*
		* construct this widget
		* 
		* @param InArgs the declaration data for this widget
		*/
		void Construct(const FArguments& in_args);

	public:

		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual FReply On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual FReply On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual glm::vec2 Compute_Desired_Size(float) const;

		/*
		* calculates the position of the color selection indicator
		* 
		* @return the position relative to the widget
		*/
		glm::vec2 calc_relative_position_from_center() const;

		/*
		* performs actions according to mouse click / move
		* 
		* @return true if the mouse action occured within the color wheel radius
		*/
		bool process_mouse_action(const FGeometry& my_geometry, const FPointerEvent& mouse_event, bool b_process_when_outside_color_wheel);

	private:
		/*the color wheel image to show*/
		const FSlateBrush* m_image;

		/*the current color selected by the user*/
		TSlateAttribute<FLinearColor, EInvalidateWidgetReason::Paint> m_selected_color;

		/*the color selector image to show*/
		const FSlateBrush* m_selector_image;

		/*invoked when the mouse is pressed and a capture begins*/
		FSimpleDelegate m_on_mouse_capture_begin;

		/*invoked when the mouse is let up and a capture ends*/
		FSimpleDelegate m_on_mouse_capture_end;

		/*invoked when a new value is selected on the color wheel*/
		FOnLinearColorValueChanged m_on_value_changed;
	};
}