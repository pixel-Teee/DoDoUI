#pragma once

#include "Children.h"
#include "FlowDirection.h"
#include "Geometry.h"
#include "SlateCore/Layout/ArrangedChildren.h"

namespace DoDo
{
	struct AlignmentArrangeResult
	{
		AlignmentArrangeResult(float in_offset, float in_size)
			: m_offset(in_offset)
			, m_size(in_size)
		{
			
		}

		float m_offset;

		float m_size;
	};

	template<EOrientation Orientation, typename SlotType>
	static AlignmentArrangeResult Align_Child(EFlowDirection in_layout_flow, float allotted_size, const SlotType& child_to_arrange, const FMargin& slot_padding,
		const float& content_scale = 1.0f, bool b_Clamp_To_Parent = true)
	{
		const FMargin& margin = slot_padding;

		const float total_margin = margin.Get_Total_Space_Along<Orientation>();

		const float margin_pre = (Orientation == Orient_Horizontal) ? margin.left : margin.top;

		const float margin_post = (Orientation == Orient_Horizontal) ? margin.right : margin.bottom;

		//todo:implement ArrangeUtils
		const int32_t alignment;

		switch(alignment)
		{
		case HAlign_Fill:
			//offset and size
			return AlignmentArrangeResult(margin_pre, std::max((allotted_size - total_margin) * content_scale, 0.0f));
		}

		const float child_desired_size = (Orientation == Orient_Horizontal) ? (child_to_arrange.get_widget()->get_desired_size().x * content_scale) : (child_to_arrange.get_widget()->get_desired_size().y * content_scale);

		//use child desired size, is fixed size
		const float child_size = std::max((b_Clamp_To_Parent ? std::max(child_desired_size, allotted_size - total_margin) : child_desired_size), 0.0f);

		switch(alignment)
		{
		case HAlign_Left://same as align_top
			return AlignmentArrangeResult(margin_pre, child_size);
		case HAlign_Center:
			return AlignmentArrangeResult((allotted_size - child_size) / 2.0f + margin_pre - margin_post, child_size);
		case HAlign_Right://same as align_bottom
			return AlignmentArrangeResult(allotted_size - child_size - margin_post, child_size);
		}

		//same as fill
		return AlignmentArrangeResult(margin_pre, std::max((allotted_size - total_margin) * content_scale, 0.0f));
	}

	//template parameter
	template<typename SlotType>
	static void Arrange_Single_Child(EFlowDirection in_flow_direction, const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, const SlotType& child_slot,
		const glm::vec2& content_scale)
	{
		//from slot to get the SWidget, then get the visibility
		const EVisibility child_visibility = child_slot.get_widget()->get_visibility();

		//todo:implement FArrangedChildren's Accepts function
		//query the arranged children's visibility filter pass have this visibility
		if(arranged_children.Accepts(child_visibility))
		{
			const glm::vec2 this_content_scale = content_scale;

			//swap padding values interm of flow direction
			const FMargin slot_padding(Layout_Padding_With_Flow(in_flow_direction, child_slot.get_padding()));

			//todo:implement FGeometry's get local size

			//todo:implement AlignmentArrangeResult
			//todo:implement FGeometry's get local size function
			//todo:to call AllottedGeometry's MakeChild, add widget to ArrangedChildren
		}
	}

	//swap padding interms of EFlowDirection
	static FMargin Layout_Padding_With_Flow(EFlowDirection in_layout_flow, const FMargin& in_padding)
	{
		FMargin return_padding(in_padding);

		if(in_layout_flow == EFlowDirection::RightToLeft)
		{
			float temp = return_padding.left;
			return_padding.left = return_padding.right;
			return_padding.right = temp;
		}

		return return_padding;
	}
}
