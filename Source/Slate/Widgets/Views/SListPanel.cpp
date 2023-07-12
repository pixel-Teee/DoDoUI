#include "PreCompileHeader.h"

#include "SListPanel.h"

#include "SlateCore/Layout/ArrangedChildren.h"//FArrangedChildren depends on it

namespace DoDo {
	//used to subtract a tiny amount from the available dimension to avoid floating point precision problems when arranging children
	static const float floating_point_precision_offset = 0.001f;

	SListPanel::SListPanel()
		: m_children(this)
	{}

	SListPanel::FSlot::FSlotArguments SListPanel::Slot()
	{
		return FSlot::FSlotArguments(std::make_unique<FSlot>());
	}
	void SListPanel::Construct(const FArguments& in_args)
	{
		m_item_width = in_args._ItemWidth;
		m_item_height = in_args._ItemHeight;
		m_num_desired_items = in_args._NumDesiredItems;
		m_item_alignment = in_args._ItemAlignment;
		m_orientation = in_args._ListOrientation;
		m_children.add_slots(std::move(const_cast<std::vector<FSlot::FSlotArguments>&>(in_args._Slots)));
	}
	FTableViewDimensions SListPanel::get_desired_item_dimensions() const
	{
		return FTableViewDimensions(m_orientation, m_item_width.Get(), m_item_height.Get());
	}
	float SListPanel::get_item_padding(const FGeometry& allotted_geometry, const EListItemAlignment list_item_alignment) const
	{
		if (list_item_alignment == EListItemAlignment::EvenlyDistributed)
		{
			FTableViewDimensions allotted_dimensions(m_orientation, allotted_geometry.get_local_size());

			FTableViewDimensions desired_dimensions = get_desired_item_dimensions();

			//only add padding between items if we have more total items that we can fit on a single line, otherwise
			//the padding around items would continue to change proportionately with the free space along the line axis
			const int32_t num_items_per_line = desired_dimensions.m_line_axis > 0.0f ? static_cast<int32_t>(allotted_dimensions.m_line_axis / desired_dimensions.m_line_axis) : 0.0f;

			if (num_items_per_line > 0 && m_children.num() > num_items_per_line)
			{
				return (allotted_dimensions.m_line_axis - floating_point_precision_offset - (num_items_per_line * desired_dimensions.m_line_axis)) / num_items_per_line;
			}
		}

		return 0.0f;
	}
	float SListPanel::get_line_padding(const FGeometry& allotted_geometry, const int32_t line_start_index) const
	{
		const int32_t num_items_left = m_children.num() - line_start_index;
		if (num_items_left <= 0)
		{
			return 0.0f;
		}

		const FTableViewDimensions allotted_dimensions(m_orientation, allotted_geometry.get_local_size());
		const FTableViewDimensions item_size = get_item_size(allotted_geometry);

		const int32_t max_num_items_on_line = item_size.m_line_axis > 0 ? static_cast<int32_t>(allotted_dimensions.m_line_axis / item_size.m_line_axis) : 0;
		const int32_t num_items_on_line = std::min(num_items_left, max_num_items_on_line);

		return allotted_dimensions.m_line_axis - floating_point_precision_offset - (num_items_on_line * item_size.m_line_axis);
	}
	FTableViewDimensions SListPanel::get_item_size(const FGeometry& allotted_geometry, const EListItemAlignment list_item_alignment) const
	{
		const FTableViewDimensions allotted_dimensions(m_orientation, allotted_geometry.get_local_size());
		const FTableViewDimensions item_dimensions = get_desired_item_dimensions();

		//only add padding between items if we have more total items that we fit on a single line, otherwise,
		//the padding around items would continue to change proportionately with the free space in our minor axis
		const int32_t num_items_per_line = item_dimensions.m_line_axis > 0 ? static_cast<int32_t>(allotted_dimensions.m_line_axis / item_dimensions.m_line_axis) : 0.0f;

		FTableViewDimensions extra_dimensions(m_orientation);

		if (num_items_per_line > 0)
		{
			if (list_item_alignment == EListItemAlignment::Fill ||
				list_item_alignment == EListItemAlignment::EvenlyWide ||
				list_item_alignment == EListItemAlignment::EvenlySize)
			{
				extra_dimensions.m_line_axis = (allotted_dimensions.m_line_axis - floating_point_precision_offset - num_items_per_line * item_dimensions.m_line_axis) / num_items_per_line;

				if (list_item_alignment == EListItemAlignment::EvenlySize)
				{
					extra_dimensions.m_scroll_axis = item_dimensions.m_scroll_axis * (extra_dimensions.m_line_axis / (item_dimensions.m_line_axis + extra_dimensions.m_line_axis));
				}
			}
		}

		return item_dimensions + extra_dimensions;
	}
	FTableViewDimensions SListPanel::get_item_size(const FGeometry& allotted_geometry) const
	{
		return get_item_size(allotted_geometry, m_item_alignment.Get());
	}
	bool SListPanel::should_arrange_as_tiles() const
	{
		FTableViewDimensions desired_item_dimensions = get_desired_item_dimensions();

		return desired_item_dimensions.m_line_axis > 0 && desired_item_dimensions.m_scroll_axis > 0;
	}
	void SListPanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		if (m_children.num() > 0)
		{
			const FTableViewDimensions allotted_dimensions(m_orientation, allotted_geometry.get_local_size());

			FTableViewDimensions dimension_so_far(m_orientation);

			if (should_arrange_as_tiles())
			{
				//this is a tile view list
				//arrange items side by side along the line axis until there is no more room, then create a new line along the scroll axis
				const EListItemAlignment list_item_alignment = m_item_alignment.Get();
				const float item_padding = get_item_padding(allotted_geometry, list_item_alignment);

				const float half_item_padding = item_padding * 0.5f;

				const FTableViewDimensions local_item_size = get_item_size(allotted_geometry, list_item_alignment);

				dimension_so_far.m_scroll_axis = -static_cast<int32_t>(m_first_line_scroll_offset * local_item_size.m_scroll_axis) - m_over_scroll_amount;

				bool b_is_new_line = true;
				for (int32_t item_index = 0; item_index < m_children.num(); ++item_index)
				{
					if(b_is_new_line)
					{
						if (list_item_alignment == EListItemAlignment::RightAligned || list_item_alignment == EListItemAlignment::CenterAligned)
						{
							const float line_padding = get_line_padding(allotted_geometry, item_index);
							const bool is_visible = m_children[item_index].get_widget()->get_visibility().is_visible();
							if (list_item_alignment == EListItemAlignment::RightAligned)
							{
								dimension_so_far.m_line_axis += is_visible ? line_padding : 0.0f;
							}
							else
							{
								const float half_line_padding = line_padding * 0.5f;
								dimension_so_far.m_line_axis += is_visible ? half_line_padding : 0.0f;
							}
						}

						b_is_new_line = false;
					}

					FTableViewDimensions child_offset = dimension_so_far;
					child_offset.m_line_axis += half_item_padding;
					arranged_children.add_widget(allotted_geometry.make_child(m_children[item_index].get_widget(), child_offset.to_vector_2d(), local_item_size.to_vector_2d()));

					dimension_so_far.m_line_axis += local_item_size.m_line_axis + item_padding;
					if (dimension_so_far.m_line_axis + local_item_size.m_line_axis + item_padding > allotted_dimensions.m_line_axis)
					{
						dimension_so_far.m_line_axis = 0;
						dimension_so_far.m_scroll_axis += local_item_size.m_scroll_axis;
						b_is_new_line = true;
					}
				}
			}
			else
			{
				//this is a normal list, arrange items in a line along the scroll axis
				const FTableViewDimensions first_child_dimensions(m_orientation, m_children[0].get_widget()->get_desired_size());

				dimension_so_far.m_scroll_axis = -static_cast<int32_t>(m_first_line_scroll_offset * first_child_dimensions.m_scroll_axis) - m_over_scroll_amount;

				for (int32_t item_index = 0; item_index < m_children.num(); ++item_index)
				{
					const FTableViewDimensions widget_desired_dimensions(m_orientation, m_children[item_index].get_widget()->get_desired_size());
					const bool b_is_visible = m_children[item_index].get_widget()->get_visibility().is_visible();

					FTableViewDimensions final_widget_dimensions(m_orientation);
					final_widget_dimensions.m_scroll_axis = b_is_visible ? widget_desired_dimensions.m_scroll_axis : 0.0f;
					final_widget_dimensions.m_line_axis = allotted_dimensions.m_line_axis;

					arranged_children.add_widget(
						allotted_geometry.make_child(m_children[item_index].get_widget(), dimension_so_far.to_vector_2d(), final_widget_dimensions.to_vector_2d())
					);

					dimension_so_far.m_scroll_axis += final_widget_dimensions.m_scroll_axis;
				}
			}
		}
	}
	glm::vec2 SListPanel::Compute_Desired_Size(float) const
	{
		FTableViewDimensions desired_list_panel_dimensions(m_orientation);

		if (should_arrange_as_tiles())
		{
			FTableViewDimensions item_dimensions = get_desired_item_dimensions();

			desired_list_panel_dimensions.m_line_axis = item_dimensions.m_line_axis;
			desired_list_panel_dimensions.m_scroll_axis = item_dimensions.m_scroll_axis;
		}
		else if (m_children.num() > 0)
		{
			//simply the sum of all the chilren along the scroll axis and the largest width along the line axis
			for (int32_t item_index = 0; item_index < m_children.num(); ++item_index)
			{
				//notice that we do not respect child visibility - it's not useful for list panels
				FTableViewDimensions child_dimensions(m_orientation, m_children[item_index].get_widget()->get_desired_size());

				desired_list_panel_dimensions.m_scroll_axis += child_dimensions.m_scroll_axis;
				desired_list_panel_dimensions.m_line_axis = std::max(desired_list_panel_dimensions.m_line_axis, child_dimensions.m_line_axis);
			}

			desired_list_panel_dimensions.m_scroll_axis /= m_children.num();
		}

		desired_list_panel_dimensions.m_scroll_axis *= m_preferred_num_lines;
		return desired_list_panel_dimensions.to_vector_2d();
	}
	FChildren* SListPanel::Get_Children()
	{
		return &m_children;
	}
	void SListPanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
	}
}