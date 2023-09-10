#include "PreCompileHeader.h"

#include "SScrollBar.h"

#include "SScrollBarTrack.h"

#include "SlateCore/Widgets/SBoxPanel.h"//SVerticalBox

#include "Slate/Widgets/Layout/SBox.h"

#include "SlateCore/Widgets/Images/SImage.h"//SImage

#include "Slate/Widgets/Layout/SSpacer.h"

namespace DoDo {
	void SScrollBar::Construct(const FArguments& in_args)
	{
		m_on_user_scrolled = in_args._OnUserScrolled;
		m_orientation = in_args._Orientation;
		m_drag_focus_cause = in_args._DragFocusCause;

		set_style(in_args._Style);

		const TAttribute<glm::vec2> thick_ness = in_args._Thickness.Is_Set() ? in_args._Thickness : glm::vec2(in_args._Style->m_thickness, in_args._Style->m_thickness);

		EHorizontalAlignment horizontal_alignment = m_orientation == Orient_Vertical ? HAlign_Center : HAlign_Fill;
		EVerticalAlignment vertical_alignment = m_orientation == Orient_Vertical ? VAlign_Fill : VAlign_Center;

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(FCoreStyle::get().get_brush("NoBrush"))
			.ColorAndOpacity(FLinearColor(0.2f, 0.3f, 1.0f, 1.0f))//todo:fix me
			.Padding(in_args._Padding)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.fill_height(1)
				[
					SNew(SBorder)
					.BorderImage(m_background_brush)
					.HAlign(horizontal_alignment)
					.VAlign(vertical_alignment)
					.Padding(0.0f)
					[
						SAssignNew(m_track, SScrollBarTrack)
						.Orientation(in_args._Orientation)
						.TopSlot()
						[
							SNew(SBox)
							.HAlign(horizontal_alignment)
							.VAlign(vertical_alignment)
							[
								SAssignNew(m_top_image, SImage)
								.Image(m_top_brush)
							]
						]
						.ThumbSlot()
						[
							SAssignNew(m_drag_thumb, SBorder)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.Padding(0.0f)
							[
								SAssignNew(m_thickness_spacer, SSpacer)
								.Size(thick_ness)
							]
						]
						.BottomSlot()
						[
							SNew(SBox)
							.HAlign(horizontal_alignment)
							.VAlign(vertical_alignment)
							[
								SAssignNew(m_bottom_image, SImage)
								.Image(m_bottom_brush)
							]
						]
					]
				]
			]
		);
	}
	void SScrollBar::set_state(float in_offset_fraction, float in_thumb_size_fraction)
	{
		if (m_track->distance_from_top() != in_offset_fraction || m_track->get_thumb_size_fraction() != in_thumb_size_fraction)
		{
			//note that the maximum offset depends on how many items fit per screen
			//it is 1.0f - in thumb size fraction
			m_track->set_sizes(in_offset_fraction, in_thumb_size_fraction);

			//todo:update visibility attribute
		}
	}
	FReply SScrollBar::On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
		{
			m_b_dragging_thumb = false;
			return FReply::handled().release_mouse_capture();
		}
		else
		{
			return FReply::un_handled();
		}
	}
	FReply SScrollBar::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
		{
			FGeometry thumb_geometry = find_child_geometry(my_geometry, m_drag_thumb);

			if (m_drag_thumb->is_directly_hovered())
			{
				//clicking on the scrollbar drag thumb
				if (m_orientation == Orient_Horizontal)
				{
					m_drag_grab_offset = thumb_geometry.absolute_to_local(mouse_event.get_screen_space_position()).x;
				}
				else
				{
					m_drag_grab_offset = thumb_geometry.absolute_to_local(mouse_event.get_screen_space_position()).y;
				}

				m_b_dragging_thumb = true;
			}
			else if (m_on_user_scrolled.is_bound())
			{
				//clicking in the non drag thumb area of the scrollbar
				m_drag_grab_offset = m_orientation == Orient_Horizontal ? (thumb_geometry.get_local_size().x * 0.5f) : (thumb_geometry.get_local_size().y * 0.5f);
				m_b_dragging_thumb = true;

				execute_on_user_scrolled(my_geometry, mouse_event);
			}
		}

		if (m_b_dragging_thumb)
		{
			return FReply::handled().capture_mouse(shared_from_this()).set_user_focus(shared_from_this(), m_drag_focus_cause);
		}
		else
		{
			return FReply::un_handled();
		}
	}
	FReply SScrollBar::On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (this->has_mouse_capture())
		{
			if (!(mouse_event.get_cursor_delta() == glm::vec2(0.0f, 0.0f)))
			{
				if (m_on_user_scrolled.is_bound())
				{
					execute_on_user_scrolled(my_geometry, mouse_event);
				}
				return FReply::handled();
			}
		}

		return FReply::un_handled();
	}
	void SScrollBar::set_style(const FScrollBarStyle* in_style)
	{
		const FScrollBarStyle* style = in_style;

		if (style == nullptr)
		{
			FArguments defaults;
			style = defaults._Style;
		}

		m_normal_thumb_image = &style->m_normal_thumb_image;
		m_hovered_thumb_image = &style->m_hovered_thumb_image;
		m_dragged_thumb_image = &style->m_dragged_thumb_image;

		if (m_orientation == Orient_Vertical)
		{
			m_background_brush = &style->m_horizontal_background_image;
			m_top_brush = &style->m_horizontal_top_slot_image;
			m_bottom_brush = &style->m_horizontal_bottom_slot_image;
		}
	}
	float SScrollBar::distance_from_top() const
	{
		return m_track->distance_from_top();
	}
	float SScrollBar::distance_from_bottom() const
	{
		return m_track->distance_from_bottom();
	}
	SScrollBar::SScrollBar()
		: m_b_dragging_thumb(false)
		, m_drag_grab_offset(0.0f)
	{
	}
	void SScrollBar::execute_on_user_scrolled(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		const int32_t axis_id = (m_orientation == Orient_Horizontal) ? 0 : 1;
		const FGeometry track_geometry = find_child_geometry(my_geometry, m_track);
		const float un_clamped_offset_in_track = track_geometry.absolute_to_local(mouse_event.get_screen_space_position())[axis_id] - m_drag_grab_offset;
		const float track_size_biased_by_min_thumb_size = track_geometry.get_local_size()[axis_id] - m_track->get_min_thumb_size();
		const float thumb_offset_in_track = std::clamp(un_clamped_offset_in_track, 0.0f, track_size_biased_by_min_thumb_size);
		const float thumb_offset = thumb_offset_in_track / track_size_biased_by_min_thumb_size;
		m_on_user_scrolled.execute_if_bound(thumb_offset);
	}
}