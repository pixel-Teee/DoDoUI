#pragma once

#include "SlateCore/Widgets/SPanel.h"//SPanel depends on it

#include "SlateCore/SlotBase.h"

#include "SlateCore/Layout/Children.h"//TPanelChildren depends on it

namespace DoDo {
	/*arrange 3 widgets, the top track, bottom track, and thumb*/
	class SScrollBarTrack : public SPanel
	{
	public:
		/*a list panel slot is very simple, it just stores a widget*/
		class FSlot : public TSlotBase<FSlot>
		{
		public:
			FSlot()
				: TSlotBase<FSlot>()
			{}
		};

		SScrollBarTrack()
			: m_children(this)
		{}

		SLATE_BEGIN_ARGS(SScrollBarTrack)
			: _TopSlot()
			, _ThumbSlot()
			, _BottomSlot()
			, _Orientation(Orient_Vertical)
		{

		}

		SLATE_NAMED_SLOT(FArguments, TopSlot)
			SLATE_NAMED_SLOT(FArguments, ThumbSlot)
			SLATE_NAMED_SLOT(FArguments, BottomSlot)
			SLATE_ARGUMENT(EOrientation, Orientation)
		SLATE_END_ARGS()

		/*
		* construct the widget from a declaration
		* 
		* @param InArgs declaration from which to construct the widget
		*/
		void Construct(const FArguments& in_args);

		struct FTrackSizeInfo
		{
			FTrackSizeInfo(const FGeometry& track_geometry, EOrientation in_orientation, float in_min_thumb_size, float thumb_size_as_fraction_of_track,
				float thumb_offset_as_fraction_track)
			{
				m_biased_track_size = ((in_orientation == Orient_Horizontal) ? track_geometry.get_local_size().x : track_geometry.get_local_size().y) - in_min_thumb_size;
				const float accurate_thumb_size = thumb_size_as_fraction_of_track * (m_biased_track_size);
				m_thumb_start = m_biased_track_size * thumb_offset_as_fraction_track;
				m_thumb_size = in_min_thumb_size + accurate_thumb_size;
			}
			
			float m_biased_track_size;
			float m_thumb_start;
			float m_thumb_size;
			float get_thumb_end()
			{
				return m_thumb_start + m_thumb_size;
			}
		};

		FTrackSizeInfo get_track_size_info(const FGeometry& in_track_geometry) const;

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual glm::vec2 Compute_Desired_Size(float) const;

		virtual FChildren* Get_Children();

		float distance_from_top() const;

		float distance_from_bottom() const;

		float get_thumb_size_fraction() const;

		void set_sizes(float in_thumb_offset_fraction, float in_thumb_size_fraction);

		float get_min_thumb_size() const;

	protected:
		static const int32_t TOP_SLOT_INDEX = 0;
		static const int32_t BOTTOM_SLOT_INDEX = 1;
		static const int32_t THUMB_SLOT_INDEX = 2;

		TPanelChildren<FSlot> m_children;

		float m_offset_fraction;
		float m_thumb_size_fraction;
		float m_min_thumb_size;
		bool m_b_is_always_visible = false;
		EOrientation m_orientation;
	};
}