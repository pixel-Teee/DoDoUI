#include "PreCompileHeader.h"

#include "SScrollBarTrack.h"

#include "SlateCore/Layout/ArrangedChildren.h"//FArrangedChildren depends on it

namespace DoDo {
	void SScrollBarTrack::Construct(const FArguments& in_args)
	{
		m_offset_fraction = 0.0f;
		m_thumb_size_fraction = 1.0f;//default to zero offset from the top with a full track thumb (scrolling not needed)
		m_min_thumb_size = 35;
		m_orientation = in_args._Orientation;

		m_children.add_slot(std::move(FSlot::FSlotArguments(std::make_unique<FSlot>())[in_args._TopSlot.m_widget]));
		m_children.add_slot(std::move(FSlot::FSlotArguments(std::make_unique<FSlot>())[in_args._BottomSlot.m_widget]));
		m_children.add_slot(std::move(FSlot::FSlotArguments(std::make_unique<FSlot>())[in_args._ThumbSlot.m_widget]));
	}

	SScrollBarTrack::FTrackSizeInfo SScrollBarTrack::get_track_size_info(const FGeometry& in_track_geometry) const
	{
		const float current_min_thumb_size = m_thumb_size_fraction <= 0.0f ? 0.0f : m_min_thumb_size;
		return FTrackSizeInfo(in_track_geometry, m_orientation, current_min_thumb_size, this->m_thumb_size_fraction, this->m_offset_fraction);
	}

	void SScrollBarTrack::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		const float width = allotted_geometry.m_size.x;
		const float height = allotted_geometry.m_size.y;

		FTrackSizeInfo track_size_info = this->get_track_size_info(allotted_geometry);

		//arrage top half of the track
		glm::vec2 child_size = (m_orientation == Orient_Horizontal)
			? glm::vec2(track_size_info.m_thumb_start, height)
			: glm::vec2(width, track_size_info.m_thumb_start);

		glm::vec2 child_pos(0, 0);
		arranged_children.add_widget(
			allotted_geometry.make_child(m_children[TOP_SLOT_INDEX].get_widget(), child_pos, child_size)
		);

		//arrange bottom half of the track
		child_pos = (m_orientation == Orient_Horizontal)
			? glm::vec2(track_size_info.get_thumb_end(), 0)
			: glm::vec2(0, track_size_info.get_thumb_end());

		child_size = (m_orientation == Orient_Horizontal)
			? glm::vec2(allotted_geometry.get_local_size().x - track_size_info.get_thumb_end(), height)
			: glm::vec2(width, allotted_geometry.get_local_size().y - track_size_info.get_thumb_end());

		arranged_children.add_widget(
			allotted_geometry.make_child(m_children[BOTTOM_SLOT_INDEX].get_widget(), child_pos, child_size)
		);

		//arrange the thumb
		child_pos = (m_orientation == Orient_Horizontal)
			? glm::vec2(track_size_info.m_thumb_start, 0)
			: glm::vec2(0, track_size_info.m_thumb_start);

		child_size = (m_orientation == Orient_Horizontal)
			? glm::vec2(track_size_info.m_thumb_size, height)
			: glm::vec2(width, track_size_info.m_thumb_size);

		arranged_children.add_widget(
			allotted_geometry.make_child(m_children[THUMB_SLOT_INDEX].get_widget(), child_pos, child_size)
		);
	}
	glm::vec2 SScrollBarTrack::Compute_Desired_Size(float) const
	{
		if (m_orientation == Orient_Horizontal)
		{
			const float desired_height = std::max(std::max(m_children[0].get_widget()->get_desired_size().y, m_children[1].get_widget()->get_desired_size().y),
				m_children[2].get_widget()->get_desired_size().y);
			return glm::vec2(m_min_thumb_size, desired_height);
		}
		else
		{
			const float desired_width = std::max(std::max(m_children[0].get_widget()->get_desired_size().x, m_children[1].get_widget()->get_desired_size().x),
				m_children[2].get_widget()->get_desired_size().x);
			return glm::vec2(desired_width, m_min_thumb_size);
		}
	}
	FChildren* SScrollBarTrack::Get_Children()
	{
		return &m_children;
	}
	float SScrollBarTrack::distance_from_top() const
	{
		return m_offset_fraction;
	}
	float SScrollBarTrack::distance_from_bottom() const
	{
		return 1.0f - (m_offset_fraction + m_thumb_size_fraction);
	}
	float SScrollBarTrack::get_thumb_size_fraction() const
	{
		return m_thumb_size_fraction;
	}
	void SScrollBarTrack::set_sizes(float in_thumb_offset_fraction, float in_thumb_size_fraction)
	{
		m_offset_fraction = in_thumb_offset_fraction;
		m_thumb_size_fraction = in_thumb_size_fraction;

		//if you have no thumb, then it's effectively the size of the whole track
		if (m_thumb_size_fraction == 0.0f)
		{
			m_thumb_size_fraction = 1.0;
		}
		else if (m_thumb_size_fraction > 1.0f)
		{
			m_thumb_size_fraction = 0.0f;
		}
		//todo:add invalidate
	}
	float SScrollBarTrack::get_min_thumb_size() const
	{
		return m_min_thumb_size;
	}
}