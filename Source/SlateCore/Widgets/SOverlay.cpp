#include <PreCompileHeader.h>

#include "SOverlay.h"

#include "SlateCore/Layout/ArrangedChildren.h"
#include "SlateCore/Types/PaintArgs.h"

#include "SlateCore/Layout/LayoutUtils.h"//LayoutPaddingWithFlow depends on it

namespace DoDo
{
	void SOverlay::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this
	}

	void SOverlay::FOverlaySlot::Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
	{
		TBasicLayoutWidgetSlot<FOverlaySlot>::Construct(slot_owner, std::move(in_args));
	}

	void SOverlay::FOverlaySlot::set_zorder(int32_t in_zorder)
	{
		if(SWidget* owner_widget = FSlotBase::get_owner_widget())
		{
			if(m_zorder != in_zorder)
			{
				TPanelChildren<FOverlaySlot>& owner_children = static_cast<SOverlay*>(owner_widget)->m_children;//get current slots

				//if bellow 0 or INDEX_NONE, then move it to the end
				if(in_zorder <= -1)
				{
					//we have at least one child since get owner widget returned a valid widget (parent/child relationship)
					//todo:check
					assert(owner_children.num() != 0);

					const FOverlaySlot& last_slot = owner_children[owner_children.num() - 1];//last slot

					if(&last_slot == this)
					{
						return;//we have already the last FSlot
					}

					in_zorder = last_slot.get_zorder() + 1;//stacking

					if(m_zorder == in_zorder) 
					{
						return;
					}
				}

				//zorder != -1
				int32_t child_index_to_move = 0;
				{
					bool b_found = false;
					const int32_t children_num = owner_children.num();
					for(; child_index_to_move < children_num; ++child_index_to_move)
					{
						const FOverlaySlot& cur_slot = owner_children[child_index_to_move];
						if(&cur_slot == this)
						{
							b_found = true;
							break;
						}
					}
					//this slot has to be contained by the children's owner
					assert(child_index_to_move >= 0 && child_index_to_move < owner_children.num() && b_found);
				}

				//find insert destination
				int32_t child_index_destination = 0;
				{
					const int32_t children_num = owner_children.num();
					for(; child_index_destination < children_num; ++child_index_destination)
					{
						const FOverlaySlot& cur_slot = owner_children[child_index_destination];
						if(in_zorder < cur_slot.get_zorder() && &cur_slot != this)
						{
							//insert before
							break;
						}
					}
					if(child_index_destination >= children_num)
					{
						const FOverlaySlot& cur_slot = owner_children[children_num - 1];
						if(&cur_slot == this)
						{
							//no need to move, it's already at the end
							child_index_to_move = child_index_destination;
						}
					}
				}

				m_zorder = in_zorder;

				if(child_index_to_move != child_index_destination)
				{
					if(child_index_destination > child_index_to_move)//make a empty room
					{
						--child_index_destination;
					}
					owner_children.move(child_index_to_move, child_index_destination);//todo:may need fix this
				}
			}
		}
		else
		{
			m_zorder = in_zorder;
		}
	}

	SOverlay::SOverlay()
		: m_children(this)//todo:add name
	{
		//todo:add set can tick
	}

	void SOverlay::Construct(const FArguments& in_args)
	{
		//move the FArguments slots to children
		m_children.add_slots(std::move(const_cast<std::vector<FOverlaySlot::FSlotArguments>&>(in_args._Slots)));
	}

	SOverlay::FOverlaySlot::FSlotArguments SOverlay::Slot()
	{
		return FOverlaySlot::FSlotArguments(std::make_unique<FOverlaySlot>(0));
	}

	void SOverlay::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const FOverlaySlot& cur_child = m_children[child_index];
			const EVisibility child_visibility = cur_child.get_widget()->get_visibility();
			if(arranged_children.Accepts(child_visibility))
			{
				const FMargin slot_padding(Layout_Padding_With_Flow(g_slate_flow_direction, cur_child.get_padding()));

				AlignmentArrangeResult x_result = Align_Child<Orient_Horizontal>(g_slate_flow_direction, allotted_geometry.get_local_size().x, cur_child, slot_padding);
				AlignmentArrangeResult y_result = Align_Child<Orient_Vertical>(allotted_geometry.get_local_size().y, cur_child, slot_padding);


				arranged_children.add_widget(child_visibility, allotted_geometry.make_child(
					cur_child.get_widget(),
					glm::vec2(x_result.m_offset, y_result.m_offset),
					glm::vec2(x_result.m_size, y_result.m_size)
				));
			}
		}
	}

	FChildren* SOverlay::Get_Children()
	{
		return &m_children;
	}

	int32_t SOverlay::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		FArrangedChildren arranged_children(EVisibility::visible);
		{
			//the box panel has no visualization of its own, it just visualizes its children
			Arrange_Children(allotted_geometry, arranged_children);
		}

		//because we paint multiple children, we must track the maximum layer id that they produced in case one of our parents
		//wants to an overlay for all of its contents
		int32_t max_layer_id = layer_id;
		FPaintArgs new_args = args.with_new_parent(this);//update this to pass to paint for input handle
		const bool b_children_enabled = should_be_enabled(b_parent_enabled);

		for(int32_t child_index = 0; child_index < arranged_children.num(); ++child_index)
		{
			FArrangedWidget& cur_arranged_widget = arranged_children[child_index];

			//we don't increment the first layer
			if(child_index > 0)
			{
				++max_layer_id;
			}

			const int32_t cur_widgets_max_layer_id =
				cur_arranged_widget.m_widget->paint(
					new_args,
					cur_arranged_widget.m_geometry,
					my_culling_rect,
					out_draw_elements,
					max_layer_id,
					in_widget_style,
					b_children_enabled
				);

			//todo:how to understand it?
			const int32_t overlay_slot_padding = 10;
			max_layer_id = cur_widgets_max_layer_id + std::min(std::max((cur_widgets_max_layer_id - max_layer_id) / overlay_slot_padding, 1) * overlay_slot_padding, 100);
		}

		return max_layer_id;
	}

	glm::vec2 SOverlay::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		glm::vec2 max_size(0.0f, 0.0f);
		for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
		{
			const FOverlaySlot& cur_slot = m_children[child_index];
			const EVisibility child_visibility = cur_slot.get_widget()->get_visibility();

			if(child_visibility != EVisibility::Collapsed)
			{
				glm::vec2 child_desired_size = cur_slot.get_widget()->get_desired_size() + cur_slot.get_padding().get_desired_size();//add padding desired size
				max_size.x = std::max(max_size.x, child_desired_size.x);
				max_size.y = std::max(max_size.y, child_desired_size.y);
			}
		}
		return max_size;
	}
}
