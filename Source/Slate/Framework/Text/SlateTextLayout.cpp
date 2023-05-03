#include "PreCompileHeader.h"

#include "SlateTextLayout.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depends on it

#include "SlateCore/Rendering/DrawElementPayloads.h"//ETextOverflowDirection depends on it

#include "Slate/Framework/Text/ILayoutBlock.h"//ILayoutBlock depends on it

#include "Slate/Framework/Text/ISlateRun.h"//ISlateRun depends on it

#include "Slate/Framework/Text/ISlateRunRenderer.h"//ISlateRunRenderer depends on it

namespace DoDo {

	int32_t FSlateTextLayout::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const ESlateDrawEffect draw_effects = b_parent_enabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		//the block size and offset values are pre-scaled, so we need to account for that when converting the block offsets into paint geometry
		const float inverse_scale = inverse(allotted_geometry.m_scale);

		//todo:check line view visible

		int32_t highest_layer_id = layer_id;

		for (int32_t line_index = 0; line_index < m_line_views.size(); ++line_index)
		{
			const FTextLayout::FLineView& line_view = m_line_views[line_index];

			ETextOverflowPolicy overflow_policy = m_text_overflow_policy_override.value_or(m_default_text_style.m_overflow_policy);

			//render any underlays for this line
			//todo:

			const int32_t block_debug_layer = highest_layer_id;
			const int32_t text_layer = block_debug_layer + 1;
			int32_t highest_block_layer_id = text_layer;

			const ETextJustify::Type visual_justification = calculate_line_view_visual_justification(line_view);//note:get the line view direction

			ETextOverflowDirection overflow_direction = visual_justification == ETextJustify::Left ? ETextOverflowDirection::LeftToRight : (visual_justification == ETextJustify::Right ?
				ETextOverflowDirection::RightToLeft : ETextOverflowDirection::NoOverflow);

			bool b_force_ellipsis_due_to_clipped_line = false;

			if (overflow_policy == ETextOverflowPolicy::Ellipsis && m_line_views.size() > 1)
			{
				//todo:handle this
			}

			//render every block for this line
			for (const std::shared_ptr<ILayoutBlock>& block : line_view.m_blocks)
			{
				FTextArgs text_args(line_view, block, m_default_text_style, overflow_policy, overflow_direction);

				const std::shared_ptr<ISlateRun> run = std::static_pointer_cast<ISlateRun>(block->get_run());

				int32_t highest_run_layer_id = text_layer;

				const std::shared_ptr<ISlateRunRenderer> run_renderer = std::static_pointer_cast<ISlateRunRenderer>(block->get_renderer());

				highest_run_layer_id = run->On_Paint(args, text_args, allotted_geometry, my_culling_rect, out_draw_elements, text_layer, in_widget_style, b_parent_enabled);

				highest_block_layer_id = std::max(highest_block_layer_id, highest_run_layer_id);
			}

			//render any overlays for this line

			//todo:paint high lights
		}

		return highest_layer_id;
	}
}