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
		m_orientation = in_args._Orientation;

		set_style(in_args._Style);

		const TAttribute<glm::vec2> thick_ness = in_args._Thickness.Is_Set() ? in_args._Thickness : glm::vec2(in_args._Style->m_thickness, in_args._Style->m_thickness);

		EHorizontalAlignment horizontal_alignment = m_orientation == Orient_Vertical ? HAlign_Center : HAlign_Fill;
		EVerticalAlignment vertical_alignment = m_orientation == Orient_Vertical ? VAlign_Fill : VAlign_Center;

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(FCoreStyle::get().get_brush("NoBrush"))
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
}