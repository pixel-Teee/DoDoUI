#include <PreCompileHeader.h>

#include "SScrollBox.h"

#include "SlateCore/Layout/LayoutUtils.h"

namespace DoDo {
	void SScrollPanel::Construct(const FArguments& in_args, std::vector<SScrollBox::FSlot::FSlotArguments> in_slots)
	{
		m_physical_offset = 0;
		m_children.add_slots(std::move(in_slots));
		m_orientation = in_args._Orientation;
		m_back_pad_scrolling = in_args._BackPadScrolling;
		m_front_pad_scrolling = in_args._FrontPadScrolling;
	}
	void SScrollPanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		float scroll_padding = m_orientation == Orient_Vertical ? allotted_geometry.get_local_size().y : allotted_geometry.get_local_size().x;

		float cur_child_offset = -m_physical_offset;

		cur_child_offset += m_back_pad_scrolling ? scroll_padding : 0;

		for (int32_t slot_index = 0; slot_index < m_children.num(); ++slot_index)
		{
			const SScrollBox::FSlot& this_slot = m_children[slot_index];
			const EVisibility child_visibility = this_slot.get_widget()->get_visibility();

			if (child_visibility != EVisibility::Collapsed)
			{
				if (m_orientation == Orient_Vertical)
				{
					cur_child_offset = arrange_child_vertical_and_return_offset(allotted_geometry, arranged_children, this_slot, cur_child_offset);
				}
				else
				{
					cur_child_offset = arrange_child_horizontal_and_return_offset(allotted_geometry, arranged_children, this_slot, cur_child_offset);
				}
			}
		}
	}
	glm::vec2 SScrollPanel::Compute_Desired_Size(float) const
	{
		glm::vec2 this_desired_size = glm::vec2(0.0f);

		for (int32_t slot_index = 0; slot_index < m_children.num(); ++slot_index)
		{
			const SScrollBox::FSlot& this_slot = m_children[slot_index];

			if (this_slot.get_widget()->get_visibility() != EVisibility::Collapsed)
			{
				const glm::vec2 child_desired_size = this_slot.get_widget()->get_desired_size();
				if (m_orientation == Orient_Vertical)
				{
					this_desired_size.x = std::max(child_desired_size.x, this_desired_size.x);
					this_desired_size.y += child_desired_size.y + this_slot.get_padding().Get_Total_Space_Along<Orient_Vertical>();
				}
				else
				{
					this_desired_size.x += child_desired_size.x + this_slot.get_padding().Get_Total_Space_Along<Orient_Horizontal>();
					this_desired_size.y = std::max(child_desired_size.y, this_desired_size.y);
				}
			}

			float scroll_padding = (m_orientation == Orient_Vertical) ? get_tick_space_geometry().get_local_size().y : get_tick_space_geometry().get_local_size().x;
			float& size_side_to_pad = (m_orientation == Orient_Vertical) ? this_desired_size.y : this_desired_size.x;
			size_side_to_pad += m_back_pad_scrolling ? scroll_padding : 0.0f;
			size_side_to_pad += -m_front_pad_scrolling ? scroll_padding : 0.0f;

			return this_desired_size;
		}
	}
	float SScrollPanel::arrange_child_vertical_and_return_offset(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, const SScrollBox::FSlot& this_slot, float cur_child_offset) const
	{
		const FMargin& this_padding = this_slot.get_padding();
		const glm::vec2& widget_desired_size = this_slot.get_widget()->get_desired_size();
		const float this_slot_desired_height = widget_desired_size.y + this_padding.Get_Total_Space_Along<Orient_Vertical>();

		//figure out the size and local position of the child within the slot, there is no vertical alignment, because
		//it does not make sense in a panel where items are stacked vertically end-to-end
		AlignmentArrangeResult x_alignment_result = Align_Child<Orient_Horizontal>(allotted_geometry.get_local_size().x, this_slot, this_padding);

		arranged_children.add_widget(allotted_geometry.make_child(this_slot.get_widget(), glm::vec2(x_alignment_result.m_offset, cur_child_offset + this_padding.top), glm::vec2(x_alignment_result.m_size, widget_desired_size.y)));

		return cur_child_offset + this_slot_desired_height;
	}
	float SScrollPanel::arrange_child_horizontal_and_return_offset(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, const SScrollBox::FSlot& this_slot, float cur_child_offset) const
	{
		const FMargin& this_padding = this_slot.get_padding();
		const glm::vec2& widget_desired_size = this_slot.get_widget()->get_desired_size();
		const float this_slot_desired_width = widget_desired_size.x + this_padding.Get_Total_Space_Along<Orient_Horizontal>();

		//figure out the size and local position of the child within the slot, there is no horizontal alignment, because
		//it does not make sense in a panel where items are stacked horizontally end-to-end
		AlignmentArrangeResult y_alignment_result = Align_Child<Orient_Vertical>(allotted_geometry.get_local_size().y, this_slot, this_padding);

		arranged_children.add_widget(allotted_geometry.make_child(this_slot.get_widget(), glm::vec2(cur_child_offset + this_padding.left, y_alignment_result.m_offset), glm::vec2(widget_desired_size.x, y_alignment_result.m_size)));

		return cur_child_offset + this_slot_desired_width;
	}
}