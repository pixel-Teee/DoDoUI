#include "PreCompileHeader.h"

#include "TextLayout.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

#include "Slate/Framework/Text/ShapedTextCache.h"

#include "Application/Application.h"

#include "Renderer/Renderer.h"

#include "SlateCore/Fonts/FontCache.h"

#include "Slate/Framework/Text/DefaultLayoutBlock.h"

namespace DoDo {
	FTextLayout::~FTextLayout()
	{
	}
	glm::vec2 FTextLayout::get_size() const
	{
		//todo:add FTextLayoutSize
		return glm::vec2(1.0f, 1.0f);
	}
	void FTextLayout::set_scale(float value)
	{
		//todo:check value is nan

		if (m_scale != value)
		{
			m_scale = value;

			//todo:mark dirty
		}
	}

	void FTextLayout::add_lines(const std::vector<FNewLineData>& new_lines)
	{
		for (const auto& new_line : new_lines)
		{
			FLineModel line_model(new_line.m_text);

			//todo:transform text

			for (const auto& run : new_line.m_runs)
			{
				line_model.m_runs.push_back(FRunModel(run));
			}

			m_line_models.push_back(line_model);
		}

		//if our layout is clean, then we can add this new line immediately (and efficiently)
		//if our layout is dirty, we might as well wait as the next update layout call will add it

		const int32_t first_new_line_model_index = m_line_models.size() - new_lines.size();

		for (int32_t line_model_index = first_new_line_model_index; line_model_index < m_line_models.size(); ++line_model_index)
		{
			FLineModel& line_model = m_line_models[line_model_index];
			//todo:add begin layout
		}

		for (int32_t line_model_index = first_new_line_model_index; line_model_index < m_line_models.size(); ++line_model_index)
		{
			FLineModel& line_model = m_line_models[line_model_index];

			//todo:calculate line layout

			const int32_t first_new_line_view_index = m_line_views.size() + 1;

			std::vector<std::shared_ptr<ILayoutBlock>> soft_line;
			flow_line_layout(line_model_index, 20.0f, soft_line);//todo:add get_wrapping_draw_width

			//apply the current margin to the newly added lines
			{

			}
		}
	}

	void FTextLayout::flow_layout()
	{
		//get_wrapping_draw_width()
		const float wrapping_draw_width = 20.0f;

		std::vector<std::shared_ptr<ILayoutBlock>> soft_line;
		
	}
	void FTextLayout::flow_line_layout(const int32_t line_model_index, const float wrapping_draw_width, std::vector<std::shared_ptr<ILayoutBlock>>& soft_line)
	{
		const FLineModel& line_model = m_line_models[line_model_index];

		int32_t current_run_index = 0;
		int32_t previous_block_end = 0;

		int32_t current_renderer_index = 0;

		//if (current_renderer_index >= line_model.m_runs.size())
		//{
		//	
		//}

		//then iterate over all of its runs
		create_line_view_blocks(line_model_index, -1, 0.0f, std::optional<float>(), current_run_index, current_renderer_index, previous_block_end, soft_line);
		soft_line.clear();
	}
	void FTextLayout::create_line_view_blocks(int32_t line_model_index, const int32_t stop_index, const float wrapped_line_width, const std::optional<float>& justification_width, int32_t& out_run_index, int32_t& out_renderer_index, int32_t& out_previous_block_end, std::vector<std::shared_ptr<ILayoutBlock>>& out_soft_line)
	{
		const FLineModel& line_model = m_line_models[line_model_index];

		const FRunTextContext run_text_context(ETextShapingMethod::KerningOnly, line_model.m_text_base_direction, line_model.m_shaped_text_cache);

		int16_t max_above_base_line = 0;
		int16_t max_below_base_line = 0;

		int32_t current_line_begin = out_previous_block_end;
		if (out_run_index < line_model.m_runs.size())
		{
			current_line_begin = std::max(current_line_begin, line_model.m_runs[out_run_index].get_text_range().m_begin_index);
		}

		int32_t current_line_end = stop_index;
		if (current_line_end == -1)
		{
			current_line_end = (line_model.m_runs.size() > 0) ? line_model.m_runs.back().get_text_range().m_end_index : 0;
		}

		//kerning only shaping implies ltr only text, so we can skip the bidirectional detection and splitting
		TextBiDi::ETextDirection line_text_direction = TextBiDi::ETextDirection::LeftToRight;
		std::vector<TextBiDi::FTextDirectionInfo> text_direction_infos;
		
		//todo:add compute text direction

		if (text_direction_infos.size() == 0)
		{
			TextBiDi::FTextDirectionInfo text_direction_info;
			text_direction_info.m_start_index = current_line_begin;
			text_direction_info.m_length = current_line_end - current_line_begin;
			text_direction_info.m_text_direction = TextBiDi::ETextDirection::LeftToRight;
			text_direction_infos.push_back(std::move(text_direction_info));
		}

		int32_t current_sorted_text_direction_info_index = 0;
		std::vector<TextBiDi::FTextDirectionInfo> sorted_text_direction_info = text_direction_infos;
		std::sort(sorted_text_direction_info.begin(), sorted_text_direction_info.end(), [](const TextBiDi::FTextDirectionInfo& in_first,
			const TextBiDi::FTextDirectionInfo& in_second) {
			return in_first.m_start_index < in_second.m_start_index;
		});

		FTextRange soft_line_range = FTextRange((int32_t)0x7fffffff, (int32_t)0x80000000);
		for (; out_run_index < line_model.m_runs.size();)
		{
			const FRunModel& run = line_model.m_runs[out_run_index];

			const FTextRange run_range = run.get_text_range();

			int32_t block_begin_index = std::max(out_previous_block_end, run_range.m_begin_index);
			int32_t block_stop_index = run_range.m_end_index;

			//blocks can only contain text with the same reading direction
			TextBiDi::ETextDirection block_text_direction = TextBiDi::ETextDirection::LeftToRight;
			int32_t current_text_direction_stop_index = 0;
			if (current_sorted_text_direction_info_index < sorted_text_direction_info.size())
			{
				const TextBiDi::FTextDirectionInfo& current_text_direction_info = sorted_text_direction_info[current_sorted_text_direction_info_index];
				current_text_direction_stop_index = current_text_direction_info.m_start_index + current_text_direction_info.m_length;

				block_stop_index = std::min(block_stop_index, current_text_direction_stop_index);
				block_text_direction = current_text_direction_info.m_text_direction;
			}

			if (stop_index != -1)
			{
				block_stop_index = std::min(stop_index, block_stop_index);
			}

			//have we reached the end of this bidirectional block?
			if (block_stop_index == current_text_direction_stop_index)
			{
				++current_sorted_text_direction_info_index;
			}

			const bool is_last_block = block_stop_index == stop_index;

			//add the new block
			{
				FBlockDefinition block_define;
				block_define.m_actual_range = FTextRange(block_begin_index, block_stop_index);
			

				out_soft_line.push_back(run.create_block(block_define, m_scale, FLayoutBlockTextContext(run_text_context, block_text_direction)));

				const FTextRange& block_range = out_soft_line.back()->get_text_range(); //note:this is important
				soft_line_range.m_begin_index = std::min(soft_line_range.m_begin_index, block_range.m_begin_index);
				soft_line_range.m_end_index = std::max(soft_line_range.m_end_index, block_range.m_end_index);
			}

			if (block_stop_index == run_range.m_end_index)
			{
				++out_run_index;
			}
		}

		glm::vec2 line_size(0.0f, 0.0f);

		//use a negative scroll offset since positive scrolling moves things negatively in screen space
		glm::vec2 current_offset(0.0f, 0.0f);

		{
			const float unscale_line_height = max_above_base_line + max_below_base_line;

			FTextLayout::FLineView line_view;
			line_view.m_offset = current_offset;
			line_view.m_size = line_size;
			line_view.m_text_height = unscale_line_height;
			line_view.m_justification_width = justification_width.value_or(line_view.m_size.x);
			line_view.m_range = soft_line_range;
			line_view.m_text_base_direction = line_model.m_text_base_direction;
			line_view.m_model_index = line_model_index;
			//line_view.m_blocks.push_back(out_soft_line);

			for (size_t i = 0; i < out_soft_line.size(); ++i)
			{
				line_view.m_blocks.push_back(out_soft_line[i]);
			}

			m_line_views.push_back(line_view);
		}

		//todo:modify text layout size
	}
	ETextJustify::Type FTextLayout::calculate_line_view_visual_justification(const FLineView& line_view) const
	{
		//work out the visual justification to use for this line
		ETextJustify::Type visual_justification = m_justification;

		if (line_view.m_text_base_direction == TextBiDi::ETextDirection::RightToLeft)
		{
			if (visual_justification == ETextJustify::Left)
			{
				visual_justification = ETextJustify::Right;
			}
			else if (visual_justification == ETextJustify::Right)
			{
				visual_justification = ETextJustify::Left;
			}
		}

		return visual_justification;
	}
	FTextLayout::FLineModel::FLineModel(const std::shared_ptr<DoDoUtf8String>& in_text)
		: m_text(in_text)
		, m_shaped_text_cache(FShapedTextCache::Create(Application::get().get_renderer()->get_font_cache()))
		, m_text_base_direction(TextBiDi::ETextDirection::LeftToRight)
	{
	}

	FTextLayout::FRunModel::FRunModel(const std::shared_ptr<IRun>& in_run)
		: m_run(in_run)
	{
	}
	std::shared_ptr<IRun> FTextLayout::FRunModel::get_run() const
	{
		return m_run;
	}
	FTextRange FTextLayout::FRunModel::get_text_range() const
	{
		return m_run->get_text_range();
	}
	std::shared_ptr<ILayoutBlock> FTextLayout::FRunModel::create_block(const FBlockDefinition& block_define, float in_scale, const FLayoutBlockTextContext& in_text_context) const
	{
		const FTextRange& size_range = block_define.m_actual_range;

		FTextRange run_range = m_run->get_text_range();
		return m_run->create_block(block_define.m_actual_range.m_begin_index, block_define.m_actual_range.m_end_index,
			glm::vec2(20.0f, 20.0f), in_text_context);//todo:add renderer
	}
}