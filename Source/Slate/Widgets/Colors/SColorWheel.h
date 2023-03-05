#pragma once

#include "SlateCore/Widgets/SLeafWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

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
		SLATE_ATTRIBUTE(glm::vec4, SelectedColor)

		/*invoked when the mouse is pressed and a capture begins*/
		SLATE_EVENT(FSimpleDelegate, OnMouseCaptureBegin)


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

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual glm::vec2 Compute_Desired_Size(float) const;

	private:
		/*the color wheel image to show*/
		const FSlateBrush* m_image;

		/*the color selector image to show*/
		const FSlateBrush* m_selector_image;
	};
}