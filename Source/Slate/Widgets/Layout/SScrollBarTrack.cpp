#include "PreCompileHeader.h"

#include "SScrollBarTrack.h"

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
	void SScrollBarTrack::On_ArrageChildren(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		const float width = allotted_geometry.m_size.x;
		const float height = allotted_geometry.m_size.y;


	}
	float SScrollBarTrack::distance_from_top() const
	{
		return m_offset_fraction;
	}
}