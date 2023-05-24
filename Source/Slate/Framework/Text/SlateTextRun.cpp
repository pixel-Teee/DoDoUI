#include "PreCompileHeader.h"

#include "SlateTextRun.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depends on it

#include "SlateCore/Layout/Geometry.h"//FGeometry depends on it

#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

#include "Slate/Framework/Text/DefaultLayoutBlock.h"//ILayoutBlock depends on it

#include "SlateCore/Fonts/FontCache.h"//FSlateFontCache depends on it

#include "Application/Application.h"

#include "Renderer/Renderer.h"

#include "SlateCore/Rendering/DrawElementPayloads.h"//FTextOverflowArgs depends on it

#include "Slate/Framework/Text/ShapedTextCache.h"//ShapedTextCacheUtil depends on it

#include "SlateCore/Fonts/FontMeasure.h"//FSlateFontMeasure depends on it

namespace DoDo {
	FSlateTextRun::~FSlateTextRun()
	{
	}
	std::shared_ptr<FSlateTextRun> FSlateTextRun::Create(const FRunInfo& in_run_info, const std::shared_ptr<DoDoUtf8String>& in_text, const FTextBlockStyle& style)
	{
		return std::make_shared<FSlateTextRun>(in_run_info, in_text, style);
	}
	std::shared_ptr<ILayoutBlock> FSlateTextRun::create_block(int32_t start_index, int32_t end_index, glm::vec2 size, const FLayoutBlockTextContext& text_context)
	{
		return FDefaultLayoutBlock::Create(shared_from_this(), FTextRange(start_index, end_index), size, text_context);
	}
	const std::vector<std::shared_ptr<SWidget>>& FSlateTextRun::get_children()
	{
		static std::vector<std::shared_ptr<SWidget>> no_children;
		return no_children;
	}
	FTextRange FSlateTextRun::get_text_range() const
	{
		return m_range;
	}
	int16_t FSlateTextRun::get_base_line(float scale) const
	{
		const std::shared_ptr<FSlateFontMeasure> font_measure = Application::get().get_renderer()->get_font_measure_service();
		return font_measure->get_base_line(m_style.m_font, scale);//todo:add shadow offset
	}
	int16_t FSlateTextRun::get_max_height(float scale) const
	{
		const std::shared_ptr<FSlateFontMeasure> font_measure = Application::get().get_renderer()->get_font_measure_service();
		return font_measure->get_max_character_height(m_style.m_font, scale);
	}
	glm::vec2 FSlateTextRun::measure(int32_t start_index, int32_t end_index, float scale, const FRunTextContext& text_context) const
	{
		if (end_index - start_index == 0)
		{
			return glm::vec2(0, get_max_height(scale));
		}

		//use the full text range (rather than the run range), so that text that spans will still be shaped correctly
		return ShapedTextCacheUtil::measure_shaped_text(text_context.m_shaped_text_cache, FCachedShapedTextKey(FTextRange(0, m_text->get_length()), scale, text_context, m_style.m_font),
			FTextRange(start_index, end_index), *m_text);
	}
	int32_t FSlateTextRun::On_Paint(const FPaintArgs& paint_args, const FTextArgs& text_args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const ESlateDrawEffect draw_effects = b_parent_enabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		const std::shared_ptr<ILayoutBlock>& block = text_args.m_block;
		const FTextBlockStyle& default_style = text_args.m_default_style;
		const FTextLayout::FLineView& line = text_args.m_line;

		const bool should_drop_shadow = false;//todo:add shadow 
		//const glm::vec2 block_location_offset = block->get_location_offset();
		const FTextRange block_range = block->get_text_range();
		const FLayoutBlockTextContext block_text_context = block->get_text_context();

		//the block size and offset values are pre-scaled, so we need to account for that when converting the block offsets into paint geometry
		const float inverse_scale = inverse(allotted_geometry.m_scale);

		//draw the text itself
		//todo:implement make shaped text

		//a negative shadow offset should be applied as a positive offset to the text to avoid clipping issues

		//make sure we have up-to-date shaped text to work with
		//we use the full line view range (rather than the run range) so that text that spans run will still be shaped correctly
		FShapedGlyphSequencePtr shaped_text = ShapedTextCacheUtil::get_shaped_text_sub_sequence(
			block_text_context.m_shaped_text_cache,
			FCachedShapedTextKey(line.m_range, allotted_geometry.get_accumulated_layout_transform().get_scale(), block_text_context, m_style.m_font),
			block_range,
			*m_text,
			block_text_context.m_text_direction
		);

		FTextOverflowArgs overflow_args;
		if (text_args.m_overflow_policy == ETextOverflowPolicy::Ellipsis && text_args.m_overflow_direction != ETextOverflowDirection::NoOverflow)
		{
			std::shared_ptr<FSlateFontCache> font_cache = Application::get().get_renderer()->get_font_cache();
			
			//todo:add get over flow ellipsis text
			overflow_args.m_overflow_direction = text_args.m_overflow_direction;
			overflow_args.m_b_force_ellipsis_due_to_clipped_line = text_args.m_b_force_ellipsis_due_to_clipped_line;
		}

		//todo:draw the optional shadow

		//draw the text itself
		FSlateDrawElement::make_shaped_text(
			out_draw_elements,
			++layer_id,
			allotted_geometry.to_paint_geometry(transform_vector(inverse_scale, block->get_size()), FSlateLayoutTransform(transform_point(inverse_scale, block->get_location_offset()))),
			shaped_text,
			draw_effects,
			in_widget_style.get_color_and_opacity_tint() * m_style.m_color_and_opacity.get_color(in_widget_style),
			in_widget_style.get_color_and_opacity_tint() * m_style.m_font.m_outline_settings.m_outline_color,
			overflow_args
		);

		return layer_id;
	}
	FSlateTextRun::FSlateTextRun(const FRunInfo& in_run_info, const std::shared_ptr<const DoDoUtf8String>& in_text, const FTextBlockStyle& in_style)
		: m_run_info(in_run_info)
		, m_text(in_text)
		, m_style(in_style)
		, m_range(0, m_text->get_length())
	{
	}
}