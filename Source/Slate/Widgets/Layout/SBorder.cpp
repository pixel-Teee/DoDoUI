#include <PreCompileHeader.h>

#include "SBorder.h"

#include "SlateCore/Layout/FlowDirection.h"//use g_slate_flow_direction
#include "SlateCore/Rendering/DrawElements.h"//on paint depends on it

namespace DoDo
{
	void SBorder::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this function
	}

	SBorder::SBorder()
		: m_desired_size_scale_attribute(*this, glm::vec2(1.0f, 1.0f))
		, m_show_disabled_effect_attribute(*this, true)
		, m_b_flip_for_right_to_left_flow_direction(false)
	{
	}

	void SBorder::Construct(const FArguments& in_args)
	{
		//set content scale
		Set_Content_Scale(in_args._ContentScale);

		m_child_slot
		.HAlign(in_args._HAlign)
		.VAlign(in_args._VAlign)
		[
			in_args._Content.m_widget
		];
	}

	int32_t SBorder::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//todo:get the FSlateBrush
		const FSlateBrush* brush_resource;//todo:to implement BorderImageAttribute

		const bool b_enabled = should_be_enabled(b_parent_enabled);//put function parameter to this function

		//todo:implement ESlateDrawEffect
		//todo:implement FSlateBrush

		if(m_b_flip_for_right_to_left_flow_direction && g_slate_flow_direction == EFlowDirection::RightToLeft)
		{
			//todo:implement make child
			//const FGeometry flipped_geometry = allotted_geometry.make_child()

			//todo:implement FSlateDrawElement MakeBox

			const ESlateDrawEffect draw_effects = ESlateDrawEffect::None;

			//todo:implement this function
			/*
			FSlateDrawElement::MakeBox(
				out_draw_elements,
				layer_id,
				allotted_geometry.to_paint_geometry(),
				brush_resource,
				draw_effects,
				brush_resource->get_tint()
			);
			*/
		}

		return SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id,
		                                 in_widget_style,
		                                 b_parent_enabled);
	}
}
