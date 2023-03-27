#include <PreCompileHeader.h>

#include "SBoxPanel.h"

#include "SlateCore/Layout/Geometry.h"

#include "SlateCore/Layout/LayoutUtils.h"

namespace DoDo
{
	void SBoxPanel::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this function
	}

	template<EOrientation Orientation, typename SlotType>
	static void Arrange_Children_Along(EFlowDirection in_layout_flow, const TPanelChildren<SlotType>& children, const FGeometry& allotted_geometry, FArrangedChildren& arranged_children)
	{
		//allotted space will be given to fixed-size children first
		//remaining space will be proportionately divided between stretch children(SizeRule_Stretch)
		//based on their stretch coefficient
		if(children.num() > 0)
		{
			float stretch_coefficient_total = 0.0f;
			float fixed_total = 0.0f;

			bool b_any_child_visible = false;
			//compute the sum of stretch coefficients (SizeRule_Stretch) and space required by fixed-size widgets
			//(SizeRule_Auto)
			for(int32_t child_index = 0; child_index < children.num(); ++child_index)
			{
				const SlotType& current_child = children[child_index];

				if(current_child.get_widget()->get_visibility() != EVisibility::Collapsed)
				{
					b_any_child_visible = true;
					//all widgets contribute their margin to the fixed space requirement
					fixed_total += current_child.get_padding().template Get_Total_Space_Along<Orientation>();

					if(current_child.get_size_rule() == FSizeParam::SizeRule_Stretch)
					{
						//for stretch children we save sum up the stretch coefficients
						stretch_coefficient_total += current_child.get_size_value();
					}
					else
					{
						glm::vec2 child_desired_size = current_child.get_widget()->get_desired_size();

						//auto-sized children contribute their desired size to the fixed space requirement
						const float child_size = (Orientation == Orient_Vertical)
							? child_desired_size.y
							: child_desired_size.x;

						//clamp to the max size if it was specified
						float max_size = current_child.get_max_size();
						fixed_total += max_size > 0 ? std::min(max_size, child_size) : child_size;
					}
				}
			}

			if (!b_any_child_visible)
				return;

			//the space available for SizeRule_Stretch widgets is any space that wasn't taken up by fixed-size widgets
			const float non_fixed_space = std::max(0.0f, (Orientation == Orient_Vertical)
				? allotted_geometry.get_local_size().y - fixed_total
				: allotted_geometry.get_local_size().x - fixed_total);

			float position_so_far = 0;

			//now that we have the total fixed-space requirement and the total stretch coefficients we an
			//arrange widgets top-to-bottom or left-to-right (depending on the orientation)
			for(TPanelChildrenConstIterator<SlotType> it(children, Orientation, in_layout_flow); it; ++it)
			{
				const SlotType& cur_child = *it;
				const EVisibility child_visibility = cur_child.get_widget()->get_visibility();

				//figure out the area allocated to the child in the direction of box panel
				//the are allocated to the slot is child size + the associated margin
				float child_size = 0;
				if(child_visibility != EVisibility::Collapsed)
				{
					//the size of the widget depends on it's size type
					if(cur_child.get_size_rule() == FSizeParam::SizeRule_Stretch)
					{
						if(stretch_coefficient_total > 0)
						{
							//stretch widgets get a fraction of the space remaining after all the fixed-space requirements are met
							child_size = non_fixed_space * cur_child.get_size_value() / stretch_coefficient_total;
						}
					}
					else
					{
						const glm::vec2 child_desired_size = cur_child.get_widget()->get_desired_size();

						//auto-sized widgets get their desired-size value
						child_size = (Orientation == Orient_Vertical)
							? child_desired_size.y
							: child_desired_size.x;
					}

					//clamp to the max size if it was specified
					float max_size = cur_child.get_max_size();
					if(max_size > 0)
					{
						child_size = std::min(max_size, child_size);
					}
				}

				const FMargin slot_padding(Layout_Padding_With_Flow(in_layout_flow, cur_child.get_padding()));

				glm::vec2 slot_size = (Orientation == Orient_Vertical)
					? glm::vec2(allotted_geometry.get_local_size().x, child_size + slot_padding.template Get_Total_Space_Along<Orient_Vertical>())
					: glm::vec2(child_size + slot_padding.template Get_Total_Space_Along<Orient_Horizontal>(), allotted_geometry.get_local_size().y);

				//figure out the size and local position of the child within the slot
				AlignmentArrangeResult x_alignment_result = Align_Child<Orient_Horizontal>(in_layout_flow, slot_size.x, cur_child, slot_padding);
				AlignmentArrangeResult y_alignment_result = Align_Child<Orient_Vertical>(slot_size.y, cur_child, slot_padding);

				const glm::vec2 local_position = (Orientation == Orient_Vertical)
					? glm::vec2(x_alignment_result.m_offset, position_so_far + y_alignment_result.m_offset)
					: glm::vec2(position_so_far + x_alignment_result.m_offset, y_alignment_result.m_offset);

				const glm::vec2 local_size = glm::vec2(x_alignment_result.m_size, y_alignment_result.m_size);

				arranged_children.add_widget(child_visibility, allotted_geometry.make_child(
					cur_child.get_widget(),
					local_position,
					local_size
				));

				if(child_visibility != EVisibility::Collapsed)
				{
					//offset the next child by the size of the current child and any post-child (bottom/right) margin
					position_so_far += (Orientation == Orient_Vertical) ? slot_size.y : slot_size.x;
 				}
			}
		}
	}

	void SBoxPanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		if(this->m_orientation == EOrientation::Orient_Horizontal)
		{
			//todo:implement arrange children along
			Arrange_Children_Along<EOrientation::Orient_Horizontal>(g_slate_flow_direction, this->m_children, allotted_geometry, arranged_children);
		}
		else
		{
			Arrange_Children_Along<EOrientation::Orient_Vertical>(g_slate_flow_direction, this->m_children, allotted_geometry, arranged_children);
		}
	}

	FChildren* SBoxPanel::Get_Children()
	{
		return &m_children;
	}

	/*
	 * helper to compute desired size
	 *
	 * @param Orientation template parameters that controls the orientation in which the children are layed out
	 * @param Children the children whose size we want to assess in a horizontal or vertical arrangement
	 *
	 * @return the size desired by the children given an orientation
	 */
	template<EOrientation Orientation, typename SlotType>
	static glm::vec2 compute_desired_size_for_box(const TPanelChildren<SlotType>& children)
	{
		//the desired size of this panel is the total size desired by its children plus any margins specified in this panel
		//the layout along the panel's axis is describe by the size param, while the perpendicular layout is described by the
		//alignment property
		glm::vec2 my_desired_size(0.0f, 0.0f);
		for(int32_t child_index = 0; child_index < children.num(); ++child_index)
		{
			const SlotType& cur_child = children[child_index];

			if(cur_child.get_widget()->get_visibility() != EVisibility::Collapsed)
			{
				const glm::vec2& cur_child_desired_size = cur_child.get_widget()->get_desired_size();

				if(Orientation == Orient_Vertical)
				{
					//for a vertical panel, we want to find the maximum desired width(including margin)
					//that will be the desired width of the whole panel
					my_desired_size.x = std::max(my_desired_size.x, cur_child_desired_size.x + cur_child.get_padding().template Get_Total_Space_Along<Orient_Horizontal>());

					//clamp to the max size if it was specified
					float final_child_desired_size = cur_child_desired_size.y;
					float max_size = cur_child.get_max_size();

					if(max_size > 0)
					{
						final_child_desired_size = std::min(max_size, final_child_desired_size);
					}

					my_desired_size.y += final_child_desired_size + cur_child.get_padding().template Get_Total_Space_Along<Orient_Vertical>();
				}
				else
				{
					//a horizontal panel is just a sideways vertical panel, the axes are swapped
					my_desired_size.y = std::max(my_desired_size.y, cur_child_desired_size.y + cur_child.get_padding().template Get_Total_Space_Along<Orient_Vertical>());

					//clamp to the max size if it was specified
					float final_child_desired_size = cur_child_desired_size.x;

					float max_size = cur_child.get_max_size();

					if(max_size > 0)
					{
						final_child_desired_size = std::min(max_size, final_child_desired_size);
					}

					my_desired_size.x += final_child_desired_size + cur_child.get_padding().template Get_Total_Space_Along<Orient_Horizontal>();
				}
			}
		}

		return my_desired_size;
	}

	glm::vec2 SBoxPanel::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		return (m_orientation == Orient_Horizontal)
			? compute_desired_size_for_box<Orient_Horizontal>(this->m_children)
			: compute_desired_size_for_box<Orient_Vertical>(this->m_children);
	}

	SBoxPanel::SBoxPanel(EOrientation in_orientation)
		: m_children(this)//todo:add name
		, m_orientation(in_orientation)
	{
	}

	void SHorizontalBox::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this function
	}

	void SHorizontalBox::Construct(const FArguments& in_args)
	{
		m_children.reserve(in_args._Slots.size());

		for(const FSlot::FSlotArguments& arg : in_args._Slots)
		{
			const FSlotBase::FSlotArguments& child_argument = static_cast<const FSlotBase::FSlotArguments&>(arg);//cast to related specific type

			const SBoxPanel::FSlot::FSlotArguments& box_slot_argument = static_cast<const SBoxPanel::FSlot::FSlotArguments&>(child_argument);

			m_children.add_slot(std::move(const_cast<SBoxPanel::FSlot::FSlotArguments&>(box_slot_argument)));
		}
	}

	void SVerticalBox::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this function
	}

	SVerticalBox::FSlot& SVerticalBox::get_slot(int32_t slot_index)
	{
		FSlotBase& base_slot = static_cast<FSlotBase&>(m_children[slot_index]);
		return static_cast<SVerticalBox::FSlot&>(base_slot);
	}

	const SVerticalBox::FSlot& SVerticalBox::get_slot(int32_t slot_index) const
	{
		const FSlotBase& base_slot = static_cast<const FSlotBase&>(m_children[slot_index]);
		return static_cast<const SVerticalBox::FSlot&>(base_slot);
	}

	void SVerticalBox::Construct(const FArguments& in_args)
	{
		m_children.reserve(in_args._Slots.size());

		for (const FSlot::FSlotArguments& arg : in_args._Slots)
		{
			const FSlotBase::FSlotArguments& child_slot_argument = static_cast<const FSlotBase::FSlotArguments&>(arg);
			const SBoxPanel::FSlot::FSlotArguments& box_slot_argument = static_cast<const SBoxPanel::FSlot::FSlotArguments&>(child_slot_argument);
			m_children.add_slot(std::move(const_cast<SBoxPanel::FSlot::FSlotArguments&>(box_slot_argument)));
		}
	}

}
