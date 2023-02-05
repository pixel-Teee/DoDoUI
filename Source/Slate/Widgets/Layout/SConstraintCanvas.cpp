#include <PreCompileHeader.h>

#include "SConstraintCanvas.h"

#include "SlateCore/Layout/ArrangedChildren.h"
#include "SlateCore/Types/PaintArgs.h"

namespace DoDo
{
	void SConstraintCanvas::FSlot::Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
	{
		//TSlotBase's owner is FChildren
		//note:move FSlotArguments's widget to this slot
		TSlotBase<FSlot>::Construct(slot_owner, std::move(in_args));

		if(in_args._Offset.Is_Set())
		{
			m_offset_attr = std::move(in_args._Offset);
		}

		if(in_args._Anchors.Is_Set())
		{
			m_anchors_attr = std::move(in_args._Anchors);
		}

		if(in_args._Alignment.Is_Set())
		{
			m_alignment_attr = std::move(in_args._Alignment);
		}

		if(in_args._AutoSize.Is_Set())
		{
			m_auto_size_attr = std::move(in_args._AutoSize);
		}

		m_zorder = in_args._ZOrder.value_or(m_zorder);
	}

	SConstraintCanvas::SConstraintCanvas()
		: m_children(this)
	{
	}

	SConstraintCanvas::FSlot::FSlotArguments SConstraintCanvas::Slot()
	{
		return FSlot::FSlotArguments(std::make_unique<FSlot>());
	}

	void SConstraintCanvas::Construct(const FArguments& in_args)
	{
		//sort the children based on zorder
		std::vector<FSlot::FSlotArguments>& slots = const_cast<std::vector<FSlot::FSlotArguments>&>(in_args._Slots);

		auto sort_operator = [](const FSlot::FSlotArguments& a, const FSlot::FSlotArguments& b)
		{
			int32_t a_zorder = a._ZOrder.value_or(0);
			int32_t b_zorder = b._ZOrder.value_or(0);

			//todo:fix me, compare two pointer
			return a_zorder == b_zorder ? reinterpret_cast<uint64_t>(&a) < reinterpret_cast<uint64_t>(&b) : a_zorder < b_zorder;
		};

		std::stable_sort(slots.begin(), slots.end(), sort_operator);

		//todo:implement children's add slots function
		m_children.add_slots(std::move(slots));
	}

	void SConstraintCanvas::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		FArrangedChildLayers child_layers;
		arrange_layered_children(allotted_geometry, arranged_children, child_layers);
	}

	int32_t SConstraintCanvas::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		FArrangedChildren arranged_children(EVisibility::visible);
		FArrangedChildLayers child_layers;
		//todo:arrange children
		arrange_layered_children(allotted_geometry, arranged_children, child_layers);

		//todo:implement bForwardedEnabled

		//because we paint multiple children, we must track the maximum layer id that they produces in case one of our parents
		//wants to an overlay for all of its contents
		int32_t max_layer_id = layer_id;
		int32_t child_layer_id = layer_id;

		//todo:implement this
		//const FPaintArgs new_args = args.with_new_parent(this);

		for(int32_t child_index = 0; child_index < arranged_children.num(); ++child_index)
		{
			FArrangedWidget& cur_widget = arranged_children[child_index];

			if(child_layers[child_index])
			{
				child_layer_id = max_layer_id + 1;
			}

			const int32_t current_widgets_max_layer_id = cur_widget.m_widget->paint(args, cur_widget.m_geometry, my_culling_rect, out_draw_elements,
				child_layer_id, in_widget_style, true);

			max_layer_id = std::max(max_layer_id, current_widgets_max_layer_id);
		}

		return max_layer_id;
	}

	FChildren* SConstraintCanvas::Get_Children()
	{
		return &m_children;
	}

	void SConstraintCanvas::arrange_layered_children(const FGeometry& allotted_geometry,
		FArrangedChildren& arranged_children, FArrangedChildLayers& arranged_child_layers) const
	{
		if(m_children.num() > 0)
		{
			//todo:implement explicit child zorder
			const bool b_explicit_child_zorder = true;

			float last_zorder = -FLT_MAX;//float max

			//arrange the children now in their proper z-order
			for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
			{
				const SConstraintCanvas::FSlot& current_child = m_children[child_index];//get the current slot

				const std::shared_ptr<SWidget>& current_widget = current_child.get_widget();

				const EVisibility child_visibility = current_widget->get_visibility();

				if(arranged_children.Accepts(child_visibility))
				{
					const FMargin offset = current_child.get_offset();
					const glm::vec2 alignment = current_child.get_alignment();
					const FAnchors anchors = current_child.get_anchors();

					const bool auto_size = current_child.get_auto_size();

					const FMargin anchor_pixels =
						FMargin(anchors.m_maximum.x * allotted_geometry.get_local_size().x,
							anchors.m_minimum.y * allotted_geometry.get_local_size().y,
							anchors.m_maximum.x * allotted_geometry.get_local_size().x,
							anchors.m_maximum.y * allotted_geometry.get_local_size().y
						);

					const bool b_is_horizontal_stretch = anchors.m_minimum.x != anchors.m_maximum.x;
					const bool b_is_vertical_stretch = anchors.m_minimum.y != anchors.m_maximum.y;

					const glm::vec2 slot_size = glm::vec2(offset.right, offset.bottom);//todo:?

					const glm::vec2 size = auto_size ? current_widget->get_desired_size() : slot_size;//if it is not auto size, then use fixed size

					//calculate the offset based on the pivot position
					glm::vec2 alignment_offset = size * alignment;

					//calculate the local position based on the anchor and position offset
					glm::vec2 local_position, local_size;

					//calculate the position and size based on the horizontal stretch or non-stretch
					if(b_is_horizontal_stretch)
					{
						local_position.x = anchor_pixels.left + offset.left;
						local_size.x = anchor_pixels.right - local_position.x - offset.right;
					}
					else
					{
						local_position.x = anchor_pixels.left + offset.left - alignment_offset.x;
						local_size.x = size.x;
					}

					//calculate the position and size based on the vertical stretch or non-stretch
					if(b_is_vertical_stretch)
					{
						local_position.y = anchor_pixels.top + offset.top;
						local_size.y = anchor_pixels.bottom - local_position.y - offset.bottom;
					}
					else
					{
						local_position.y = anchor_pixels.top + offset.top - alignment_offset.y;
						local_size.y = size.y;
					}

					//add information about this child to the output list(ArrangeChildren)
					arranged_children.add_widget(child_visibility, allotted_geometry.make_child(
						//the child widget beging arranged
						current_widget,
						//child's local position(i.e. position within parent)
						local_position,
						//child's size
						local_size
					));

					bool b_new_layer = true;
					if(b_explicit_child_zorder)
					{
						//split children into discrete layers for the paint method
						b_new_layer = false;
						if(current_child.get_zorder() > last_zorder + 0.00001f)
						{
							if(arranged_child_layers.size() > 0)
							{
								b_new_layer = true;
							}
							last_zorder = current_child.get_zorder();
						}
					}
					arranged_child_layers.push_back(b_new_layer);
				}
			}
		}
	}

	glm::vec2 SConstraintCanvas::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		return glm::vec2(1.0f, 1.0f);
	}
}
