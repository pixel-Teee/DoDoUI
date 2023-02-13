#include <PreCompileHeader.h>

#include "SSplitter.h"

#include "SlateCore/Layout/ArrangedChildren.h"
#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

#include "SlateCore/Styling/SlateBrush.h"//FSlateBrush
#include "SlateCore/Styling/SlateTypes.h"

namespace DoDo {
	static int32_t Clamp(const int32_t X, const int32_t Min, const int32_t Max)
	{
		return (X < Min) ? Min : (X < Max) ? X : Max;//clamp to [Min, Max]
	}

	void SSplitter::FSlot::Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
	{
		TSlotBase<FSlot>::Construct(slot_owner, std::move(in_args));

		if(in_args._Value.Is_Set())
		{
			m_size_value = std::move(in_args._Value);
		}

		if(in_args._Resizable.has_value())
		{
			m_b_is_resizable = in_args._Resizable.value();
		}

		m_min_size_value = in_args._MinSize;

		if(in_args._OnSlotResized.Is_Bound())
		{
			m_on_slot_resized_handler = std::move(in_args._OnSlotResized);
		}

		if(in_args._SizeRule.Is_Set())
		{
			m_sizing_rule = std::move(in_args._SizeRule);
		}
	}

	SSplitter::FSlot::FSlotArguments SSplitter::Slot()
	{
		return FSlot::FSlotArguments(std::make_unique<FSlot>());
	}

	SSplitter::FScopedWidgetSlotArguments SSplitter::add_slot(int32_t at_index)
	{
		return FScopedWidgetSlotArguments{ std::make_unique<FSlot>(), m_children, at_index };//when this destroy, to call destruction function
	}

	SSplitter::SSplitter()
		: m_children(this)
		, m_hovered_handle_index(-1)
		, m_highlighted_handle_index(*this, -1)
		, m_orientation(Orient_Horizontal)
	{
		
	}

	void SSplitter::Construct(const FArguments& in_args)
	{
		m_physical_splitter_handle_size = in_args._PhysicalSplitterHandleSize;
		m_hit_detection_splitter_handle_size = in_args._HitDetectionSplitterHandleSize;
		m_orientation = in_args._Orientation;
		m_hovered_handle_index = -1;
		m_highlighted_handle_index.Assign(*this, in_args._HighlightedHandleIndex);

		m_style = in_args._Style;
		m_min_splitter_child_length = in_args._MinimumSlotHeight;
		//todo:implement add slots
		m_children.add_slots(std::move(const_cast<std::vector<FSlot::FSlotArguments>&>(in_args._Slots)));
	}

	/*
	 * panels arrange their children in a space described by the allocated geometry parameter, the results of the arrangement
	 * should be returned by appending a FArrangedWidget pair for every child widget, see stack panel for an example
	 */
	void SSplitter::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		//todo:implement FLayoutGeometry and arrange children for layout
		//std::vector<FLayoutGeometry> layout_children =
		std::vector<FLayoutGeometry> layout_children = arrange_children_for_layout(allotted_geometry);

		//arrange the children horizontally or vertically
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			arranged_children.add_widget(allotted_geometry.make_child(m_children[child_index].get_widget(), layout_children[child_index]));
		}
	}

	int32_t SSplitter::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
	                            const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
	                            const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		FArrangedChildren arranged_children(EVisibility::visible);//arrange children, if we have three children, the we will have two handle
		Arrange_Children(allotted_geometry, arranged_children);//dispatch to virtual function, arrange children

		//dispatch virtual paint function
		int32_t max_layer_id = paint_arranged_children(args, arranged_children, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);

		//todo:implement FSplitterStyle
		const FSlateBrush* normal_handled_brush = &m_style->m_handled_normal_brush;

		//draw the splitter above any children
		max_layer_id += 1;

		//handle index is a index into our children array, but some of those may be hidden and missing from arranged children so we need to update the index for the loop below
		auto find_arranged_handle_index = [this, &arranged_children](int32_t handle_index)
		{
			if(handle_index == -1 || arranged_children.num() == m_children.num())
			{
				return handle_index;
			}

			const std::shared_ptr<SWidget>& hovered_child_to_find = m_children[handle_index].get_widget();

			for(int32_t i = 0; i < arranged_children.num(); ++i)
			{
				if (arranged_children[i].m_widget == hovered_child_to_find)
					return i;
			}
			return -1;
		};

		int32_t arranged_hovered_handle_index = find_arranged_handle_index(m_hovered_handle_index);

		int32_t arranged_highlighted_handle_index = find_arranged_handle_index(m_highlighted_handle_index.Get());

		for(int32_t child_index = 0; child_index < arranged_children.num() - 1; ++child_index)//draw splitter, number is children - 1
		{
			const FGeometry& geometry_after_splitter = arranged_children[Clamp(child_index + 1, 0, arranged_children.num() - 1)].m_geometry;

			const float half_hit_detection_splitter_handle_size = m_hit_detection_splitter_handle_size / 2.0f;//todo:implement Round To float
			const float half_physical_splitter_handle_size = m_physical_splitter_handle_size;

			glm::vec2 handle_size;
			glm::vec2 handle_position;
			if(m_orientation == Orient_Horizontal)
			{
				handle_size = glm::vec2(m_physical_splitter_handle_size, geometry_after_splitter.m_size.y);
				handle_position = glm::vec2(-m_physical_splitter_handle_size, 0);
			}
			else
			{
				handle_size = glm::vec2(geometry_after_splitter.m_size.x, m_physical_splitter_handle_size);
				handle_position = glm::vec2(0, -m_physical_splitter_handle_size);//todo:how to understand it?
			}

			if(arranged_hovered_handle_index == child_index || arranged_highlighted_handle_index == child_index) //note:check this splitter handle?
			{
				//handle is hovered
				FSlateDrawElement::MakeBox(
					out_draw_elements,
					max_layer_id,
					geometry_after_splitter.to_paint_geometry(handle_position, handle_size, 1.0f),//note:make a offset from this geometry
					normal_handled_brush,
					should_be_enabled(b_parent_enabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
					glm::vec4(1.0f, 1.0f, 0.2f, 1.0f)
				);
			}
			else
			{
				//handle is hovered
				FSlateDrawElement::MakeBox(
					out_draw_elements,
					max_layer_id,
					geometry_after_splitter.to_paint_geometry(handle_position, handle_size, 1.0f),
					&m_style->m_handle_highlight_brush,//todo:modify this
					should_be_enabled(b_parent_enabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
					glm::vec4(1.0f, 1.0f, 0.2f, 1.0f)
				);
			}
		}

		return max_layer_id;
	}

	std::vector<FLayoutGeometry> SSplitter::arrange_children_for_layout(const FGeometry& allotted_geometry) const
	{
		std::vector<FLayoutGeometry> result_geometries;
		//result_geometries.resize(m_children.num());

		const int32_t axis_index = (m_orientation == Orient_Horizontal) ? 0 : 1;

		// Splitters divide the space between their children proportionately based on size coefficients
		// the size coefficients are usually determined by a user, who grabs the handled between the child elements
		// and moves them to resize the space available to the children
		// some children are sized automatically based on their content, those children cannot be resized
		// 
		// e.g.   _____________________________________ Children
		//       /              /                  /
		//      v              v                  v
		//   + - - - - - + + - - - + + - - - - - - - - - - - - - - +
		//   |           | |       | |                             |
		//   | Child 0   | |Child1 | |  Child2                     |
		//   + - - - - - + + - - - + + - - - - - - - - - - - - - - +
		//                ^         ^
		//                 \_________\___________ Resize handles.

		int32_t num_non_collapsed_children = 0;
		int32_t num_resizeable_children = 0;
		float coefficient_total = 0;
		//some space is claimed by non-resizable elements(auto-sized elements)
		float non_resizable_space = 0;
		float min_resizable_space = 0;

		auto is_slot_collapsed = [](const FSlot& slot)
		{
			return slot.get_widget()->get_visibility() == EVisibility::Collapsed;
		};

		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const FSlot& cur_slot = m_children[child_index];

			if(is_slot_collapsed(cur_slot))
			{
				continue;
			}

			++num_non_collapsed_children;

			if(cur_slot.m_sizing_rule.Get() == SSplitter::SizeToContent)
			{
				non_resizable_space += cur_slot.get_widget()->get_desired_size()[axis_index];
			}
			else //SizingRule == SSplitter::FractionOfParent
			{
				min_resizable_space += std::min(m_min_splitter_child_length, cur_slot.get_min_size());
				coefficient_total += cur_slot.m_size_value.Get();
			}
		}

		/*the user-sizable children must make room for the resize handles and for auto-sized children*/
		const float space_needed_for_handles = std::max(0, num_non_collapsed_children - 1) * m_physical_splitter_handle_size;
		const float resizable_space = allotted_geometry.m_size[axis_index] - space_needed_for_handles - non_resizable_space;

		std::vector<float> slot_sizes;
		//slot_sizes.resize(m_children.num());

		float extra_required_space = 0;

		//calculate slot sizes
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const FSlot& cur_slot = m_children[child_index];

			//slot is collapsed, size is automatically 0
			if(is_slot_collapsed(cur_slot))
			{
				slot_sizes.push_back(0);
				continue;
			}

			float child_space;

			if(cur_slot.m_sizing_rule.Get() == SSplitter::SizeToContent)
			{
				child_space = cur_slot.get_widget()->get_desired_size()[axis_index];
			}
			else
			{
				child_space = resizable_space * cur_slot.m_size_value.Get() / coefficient_total - extra_required_space;
				extra_required_space = 0;
			}

			//we have enough space to clamp this and steal space from earlier slots
			if(resizable_space >= min_resizable_space)
			{
				const float clamped_space = clamp_child(cur_slot, child_space);//max[slot.min_size, splitter_handle.length, child_space]
				float current_required_space = clamped_space - child_space;

				for(int32_t prev_index = child_index - 1; prev_index >= 0 && current_required_space > 0; --prev_index)
				{
					if(m_children[prev_index].m_sizing_rule.Get() == SSplitter::FractionOfParent && !is_slot_collapsed(cur_slot))
					{
						const float min_child_size = std::max(m_min_splitter_child_length, m_children[prev_index].get_min_size());
						const float available_space = slot_sizes[prev_index] - min_child_size;
						if(available_space > 0)
						{
							const float space_to_steal = std::min(available_space, current_required_space);
							slot_sizes[prev_index] -= space_to_steal;
							current_required_space -= space_to_steal;
						}
					}
				}

				if(current_required_space > 0)
				{
					//we weren't able to steal enough space from previous children, store this value to try and take it from following children
					extra_required_space = current_required_space;
				}

				child_space = clamped_space;
			}

			slot_sizes.push_back(child_space);
		}

		//create geometries and calculate offsets
		float offset = 0;
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const glm::vec2 child_offset = m_orientation == Orient_Horizontal ? glm::vec2(offset, 0) : glm::vec2(0, offset);
			const glm::vec2 child_size = m_orientation == Orient_Horizontal ? glm::vec2(slot_sizes[child_index], allotted_geometry.m_size.y) : glm::vec2(allotted_geometry.m_size.x, slot_sizes[child_index]);

			result_geometries.emplace_back(FSlateLayoutTransform(child_offset), child_size);

			//advance to the next slot, if the child is collapsed, it takes up no room and does not need a splitter
			if(!is_slot_collapsed(m_children[child_index]))
			{
				offset += slot_sizes[child_index] + m_physical_splitter_handle_size;//todo:implement round to int
			}
		}

		return result_geometries;
	}

	/*
	 * @param ProposedSize a size that a child would like to be
	 *
	 * @return a size that is clamped against the minimum size allowed for children
	 */

	float SSplitter::clamp_child(const FSlot& child_slot, float proposed_size) const
	{
		float clamped_size = std::max(m_min_splitter_child_length, proposed_size);
		clamped_size = std::max(child_slot.get_min_size(), proposed_size);
		return clamped_size;
	}

	glm::vec2 SSplitter::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		return glm::vec2(100.0f, 100.0f);//todo:fix me
	}

	FChildren* SSplitter::Get_Children()
	{
		return &m_children;
	}
}
