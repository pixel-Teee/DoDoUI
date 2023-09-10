#pragma once

#include "Slate/Widgets/Layout/SBorder.h"

#include "SlateCore/Styling/SlateTypes.h"//FScrollBarStyle depends on it

namespace DoDo {
	DECLARE_DELEGATE_OneParam(
		FOnUserScrolled,
		float
	); //scroll offset as a fraction between 0 and 1

	class SScrollBarTrack;
	class SSpacer;
	class SImage;
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
			SLATE_EVENT(FOnUserScrolled, OnUserScrolled)

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

		/*
		* set the offset and size of the track's thumb
		* note that the maximum offset is 1.0 - thumb size fraction
		* if the user can view 1/3 of the items in a single page, the maximum offset will be ~0.667f
		* 
		* @param InOffsetFraction offset of the thumbnail from the top as a fraction of the total available scroll space
		* @param InThumbSizeFraction Size of thumbnail as a fraction of the total available scroll space
		*/
		void set_state(float in_offset_fraction, float in_thumb_size_fraction);

		virtual FReply On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
		virtual FReply On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		/*set argument style*/
		void set_style(const FScrollBarStyle* in_style);

		/*@return normalized distance from top*/
		float distance_from_top() const;

		float distance_from_bottom() const;

		SScrollBar();
	protected:

		/*execute the on user scrolled delegate*/
		void execute_on_user_scrolled(const FGeometry& my_geometry, const FPointerEvent& mouse_event);

		std::shared_ptr<SImage> m_top_image;

		std::shared_ptr<SImage> m_bottom_image;

		std::shared_ptr<SBorder> m_drag_thumb;

		bool m_b_dragging_thumb;

		EFocusCause m_drag_focus_cause;

		std::shared_ptr<SSpacer> m_thickness_spacer;

		std::shared_ptr<SScrollBarTrack> m_track;
		FOnUserScrolled m_on_user_scrolled;
		EOrientation m_orientation;

		float m_drag_grab_offset;

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