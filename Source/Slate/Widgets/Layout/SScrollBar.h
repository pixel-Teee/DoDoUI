#pragma once

#include "Slate/Widgets/Layout/SBorder.h"

#include "SlateCore/Styling/SlateTypes.h"//FScrollBarStyle depends on it

namespace DoDo {
	class SScrollBarTrack;
	class SScrollBar : public SBorder
	{
	public:
		SLATE_BEGIN_ARGS(SScrollBar)
			: _Style(&FAppStyle::get().get_widget_style<FScrollBarStyle>("Scrollbar"))
			, _Orientation(Orient_Vertical)
			, _DragFocusCause(EFocusCause::Mouse)
			, _Thickness()
			, _Padding(2.0f)
		{}
			/*the style to use for this scrollbar*/
			SLATE_STYLE_ARGUMENT(FScrollBarStyle, Style)
			//todo:add FOnUserScrolled

			SLATE_ARGUMENT(EOrientation, Orientation)
			SLATE_ARGUMENT(EFocusCause, DragFocusCause)
			/*the thickness of the scrollbar thumb*/
			SLATE_ATTRIBUTE(glm::vec2, Thickness)
			/*the margin around the scrollbar*/
			SLATE_ATTRIBUTE(FMargin, Padding)
		SLATE_END_ARGS()

		/*
		* construct this widget
		* 
		* @param InArgs the declaration data for this widget
		*/
		void Construct(const FArguments& in_args);

		/*set argument style*/
		void set_style(const FScrollBarStyle* in_style);

		/*@return normalized distance from top*/
		float distance_from_top() const;

		float distance_from_bottom() const;
	protected:

		std::shared_ptr<SScrollBarTrack> m_track;
		EOrientation m_orientation;

		/*image to use when the scrollbar thumb is in its normal state*/
		const FSlateBrush* m_normal_thumb_image;

		/*image to use when the scrollbar thumb is in its hovered state*/
		const FSlateBrush* m_hovered_thumb_image;

		/*image to use when the scrollbar thumb is in its dragged state*/
		const FSlateBrush* m_dragged_thumb_image;

		/*background brush*/
		const FSlateBrush* m_background_brush;

		/*top brush*/
		const FSlateBrush* m_top_brush;

		/*bottom brush*/
		const FSlateBrush* m_bottom_brush;
	};
}