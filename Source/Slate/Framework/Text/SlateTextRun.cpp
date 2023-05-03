#include "PreCompileHeader.h"

#include "SlateTextRun.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depends on it

#include "SlateCore/Layout/Geometry.h"//FGeometry depends on it

namespace DoDo {
	FSlateTextRun::~FSlateTextRun()
	{
	}
	int32_t FSlateTextRun::On_Paint(const FPaintArgs& paint_args, const FTextArgs& text_args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const ESlateDrawEffect draw_effects = b_parent_enabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		const std::shared_ptr<ILayoutBlock>& block = text_args.m_block;
		const FTextBlockStyle& default_style = text_args.m_default_style;
		const FTextLayout::FLineView& line = text_args.m_line;

		const bool should_drop_shadow = false;//todo:add shadow 
		//const glm::vec2 block_location_offset = block->get_location_offset();
		//const FTextRange block_range = block->get_text_range();

		//the block size and offset values are pre-scaled, so we need to account for that when converting the block offsets into paint geometry
		const float inverse_scale = inverse(allotted_geometry.m_scale);

		//draw the text itself
		//todo:implement make shaped text

		return layer_id;
	}
}