#include <PreCompileHeader.h>

#include "SNodePanel.h"

#include "SlateCore/Styling/SlateBrush.h"//FSlateBrush depends on it

namespace DoDo {
	void SNodePanel::SNode::FNodeSlot::Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
	{
		TSlotBase<FNodeSlot>::Construct(slot_owner, std::move(in_args));
		TAlignmentWidgetSlotMixin<FNodeSlot>::Construct_Mixin(slot_owner, std::move(in_args));

		if (in_args._Padding.Is_Set())
		{
			m_slot_padding = std::move(in_args._Padding);
		}

		if (in_args._SlotOffset.Is_Set())
		{
			m_offset = std::move(in_args._SlotOffset);
		}

		if (in_args._SlotSize.Is_Set())
		{
			m_size = std::move(in_args._SlotSize);
		}

		if (in_args._AllowScaling.Is_Set())
		{
			m_allow_scale = std::move(in_args._AllowScaling);
		}
	}
	SNodePanel::SNodePanel()
		: m_children(this)
		, m_visible_children(this)
	{
	}

	void SNodePanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		//todo:implement this function
	}

	glm::vec2 SNodePanel::Compute_Desired_Size(float) const
	{
		return glm::vec2(160.0f, 160.0f);
	}

	FChildren* SNodePanel::Get_Children()
	{
		return &m_visible_children;
	}

	float SNodePanel::get_zoom_amount() const
	{
		return 1.0f;//todo:fix me
	}

	void SNodePanel::paint_background_as_lines(const FSlateBrush* background_image, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t& draw_layer_id) const
	{
		const bool b_antialias = false;

		const FLinearColor graph_background_image_color(background_image->m_tint_color.get_specified_color());

	}
}