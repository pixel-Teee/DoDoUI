#include "PreCompileHeader.h"

#include "TextLayout.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

#include "Slate/Framework/Text/ShapedTextCache.h"

#include "Application/Application.h"

#include "Renderer/Renderer.h"

#include "SlateCore/Fonts/FontCache.h"

#include "Slate/Framework/Text/DefaultLayoutBlock.h"

namespace DoDo {
	FTextLayout::FTextLayout()
		: m_wrapping_width(0)
		, m_transform_policy(ETextTransformPolicy::None)
		, m_wrapping_policy(ETextWrappingPolicy::DefaultWrapping)
		, m_scale(1.0f)
		, m_justification(ETextJustify::Left)
	{
	}
	FTextLayout::~FTextLayout()
	{
	}
	glm::vec2 FTextLayout::get_size() const
	{
		//todo:add FTextLayoutSize
		return m_text_layout_size.get_draw_size() * inverse(m_scale);
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

	void FTextLayout::set_wrapping_width(float value)
	{
		const bool was_wrapping = m_wrapping_width > 0.0f;
		const bool is_wrapping = value > 0.0f;

		if (m_wrapping_width != value)
		{
			m_wrapping_width = value;

			//todo:mark dirty
		}
	}

	void FTextLayout::set_transform_policy(ETextTransformPolicy value)
	{
		if (m_transform_policy != value)
		{
			m_transform_policy = value;
		}
	}

	void FTextLayout::clear_lines()
	{
		m_line_models.clear();
		clear_view();
	}

	void FTextLayout::add_lines(const std::vector<FNewLineData>& new_lines)
	{
		for (const auto& new_line : new_lines)
		{
			FLineModel line_model(new_line.m_text);

			//todo:transform text

			for (const auto& run : new_line.m_runs)
			{
				line_model.m_runs.push_back(FRunModel(run)); //construct FRunModel
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
			flow_line_layout(line_model_index, get_wrapping_draw_width(), soft_line);//todo:add get_wrapping_draw_width

			//apply the current margin to the newly added lines
			{

			}
		}
	}

	bool FTextLayout::insert_at(const FTextLocation& location, DoDoUtf8String character)
	{
		const int32_t insert_location = location.get_offset();
		const int32_t line_index = location.get_line_index();

		if (!(line_index >= 0 && line_index < m_line_models.size()))
		{
			return false;
		}

		FLineModel& line_model = m_line_models[line_index];

		line_model.m_text->insert_at(insert_location, character);
		//todo:add dirty mark

		bool run_is_after_insert_location = false;
		for (int32_t run_index = 0; run_index < line_model.m_runs.size(); ++run_index)
		{
			FRunModel& run_model = line_model.m_runs[run_index];

			const FTextRange& run_range = run_model.get_text_range();

			const bool b_is_last_run = run_index == line_model.m_runs.size() - 1;

			if (run_range.contains(insert_location) || (b_is_last_run && !run_is_after_insert_location))
			{
				run_is_after_insert_location = true;

				run_model.set_text_range(FTextRange(run_range.m_begin_index, run_range.m_end_index + 1));

				//todo:add more logic
			}
			else if(run_is_after_insert_location)
			{
				FTextRange new_range = run_range;
				new_range.offset(1);
				run_model.set_text_range(new_range);
			}
		}

		return true;
	}

	void FTextLayout::update_if_needed()
	{
		update_layout();
	}

	void FTextLayout::update_layout()
	{
		clear_view();

		flow_layout();
	}

	void FTextLayout::clear_view()
	{
		m_text_layout_size = FTextLayoutSize();
		m_line_views.clear();
	}

	float FTextLayout::get_wrapping_draw_width() const
	{
		return std::max(0.01f, (m_wrapping_width)); //todo:add margin
	}

	void FTextLayout::flow_layout()
	{
		//get_wrapping_draw_width()
		const float wrapping_draw_width = get_wrapping_draw_width();

		std::vector<std::shared_ptr<ILayoutBlock>> soft_line;

		for (int32_t line_model_index = 0; line_model_index < m_line_models.size(); ++line_model_index)
		{
			FLineModel& line_model = m_line_models[line_model_index];

			flow_line_layout(line_model_index, wrapping_draw_width, soft_line);
		}
		
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

		const bool is_wrapping = m_wrapping_width > 0.0f;

		//if the line doesn't have any break candidates, or we're not wrapping text
		if (!is_wrapping) //todo:add break candidates
		{
			//then iterate over all of its runs
			create_line_view_blocks(line_model_index, -1, 0.0f, std::optional<float>(), /*out*/current_run_index, /*out*/current_renderer_index, /*out*/previous_block_end, soft_line);
			soft_line.clear();
		}
		else
		{

		}
		
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

		//ensure there is at least one directional block, this can happen when using kerning only shaping (since we skip the bidirectional detection), or for empty strings that are run through the bidirectional detection
		if (text_direction_infos.size() == 0)
		{
			TextBiDi::FTextDirectionInfo text_direction_info;
			text_direction_info.m_start_index = current_line_begin;
			text_direction_info.m_length = current_line_end - current_line_begin;
			text_direction_info.m_text_direction = TextBiDi::ETextDirection::LeftToRight;
			text_direction_infos.push_back(std::move(text_direction_info));
		}

		//we always add the runs to the line in ascending index order, so re-order a copy of text direction data so we can iterater it forwards by ascending indedx
		//we'll re-sort the line into the correct visual order once we've finished generating the blocks
		int32_t current_sorted_text_direction_info_index = 0;
		std::vector<TextBiDi::FTextDirectionInfo> sorted_text_direction_info = text_direction_infos;
		std::sort(sorted_text_direction_info.begin(), sorted_text_direction_info.end(), [](const TextBiDi::FTextDirectionInfo& in_first,
			const TextBiDi::FTextDirectionInfo& in_second) {
			return in_first.m_start_index < in_second.m_start_index;
		});

		FTextRange soft_line_range = FTextRange((int32_t)0x7fffffff, (int32_t)0x80000000); //max_int32 and min_int32
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
			
				//call FRunModel's create block 
				out_soft_line.push_back(run.create_block(block_define, m_scale, FLayoutBlockTextContext(run_text_context, block_text_direction)));

				//update the soft line bounds based on this new block (needed within this loop due to bi-directional text, as the extends of the line array are not always the start and end of the range)
				const FTextRange& block_range = out_soft_line.back()->get_text_range(); //note:this is important
				soft_line_range.m_begin_index = std::min(soft_line_range.m_begin_index, block_range.m_begin_index);
				soft_line_range.m_end_index = std::max(soft_line_range.m_end_index, block_range.m_end_index);
			}

			//get the base line and flip it's sign, baselines are generally negative
			const int16_t base_line = -(run.get_base_line(m_scale));

			//for the height of the slice we need to take into account the largest value below and above the baseline and add those together
			max_above_base_line = std::max(max_above_base_line, (int16_t)(run.get_max_height(m_scale) - base_line));
			max_below_base_line = std::max(max_below_base_line, base_line);

			if (block_stop_index == run_range.m_end_index)
			{
				++out_run_index;
			}

			if (is_last_block)
			{
				break;
			}
		}

		glm::vec2 line_size(0.0f, 0.0f);

		//use a negative scroll offset since positive scrolling moves things negatively in screen space
		glm::vec2 current_offset(0.0f, 0.0f);//todo:fix me

		{
			float current_horizontal_pos = 0.0f;

			for (int32_t index = 0; index < out_soft_line.size(); ++index)
			{
				const std::shared_ptr<ILayoutBlock> block = out_soft_line[index];
				const std::shared_ptr<IRun> run = block->get_run();

				const int16_t block_base_line = run->get_base_line(m_scale);
				const int16_t vertical_offset = max_above_base_line - block->get_size().y - block_base_line;
				//const int8_t block_kerning = run->

				//todo:add kerning

				block->set_location_offset(glm::vec2(current_offset.x + current_horizontal_pos, current_offset.y + vertical_offset));

				current_horizontal_pos += block->get_size().x;
			}

			const float unscale_line_height = max_above_base_line + max_below_base_line;

			line_size.x = current_horizontal_pos;
			line_size.y = unscale_line_height;//todo:add line height percentage

			FTextLayout::FLineView line_view;
			line_view.m_offset = current_offset;
			line_view.m_size = line_size;
			line_view.m_text_height = unscale_line_height;
			line_view.m_justification_width = justification_width.value_or(line_view.m_size.x);
			line_view.m_range = soft_line_range;
			line_view.m_text_base_direction = line_model.m_text_base_direction;
			line_view.m_model_index = line_model_index;
			//line_view.m_blocks.push_back(out_soft_line);

			for (size_t i = 0; i < out_soft_line.size(); ++i) //note:append
			{
				line_view.m_blocks.push_back(out_soft_line[i]);
			}

			m_line_views.push_back(line_view);
		}

		//todo:modify text layout size

		m_text_layout_size.m_draw_width = std::max(m_text_layout_size.m_draw_width, line_size.x);//draw width is the size of the longest line + the margin
		m_text_layout_size.m_wrapped_width = std::max(m_text_layout_size.m_wrapped_width, (stop_index == -1) ? line_size.x : m_wrapping_width);//wrapped width is the size of the longest line + any trailing whitespace width
		m_text_layout_size.m_height += line_size.y;//height is the total height of all lines
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
	void FTextLayout::FRunModel::set_text_range(const FTextRange& value)
	{
		m_run->set_text_range(value);
	}
	int16_t FTextLayout::FRunModel::get_base_line(float scale) const
	{
		return m_run->get_base_line(scale);
	}
	int16_t FTextLayout::FRunModel::get_max_height(float scale) const
	{
		return m_run->get_max_height(scale);
	}
	std::shared_ptr<ILayoutBlock> FTextLayout::FRunModel::create_block(const FBlockDefinition& block_define, float in_scale, const FLayoutBlockTextContext& in_text_context) const
	{
		const FTextRange& size_range = block_define.m_actual_range;

		FTextRange run_range = m_run->get_text_range();
		return m_run->create_block(block_define.m_actual_range.m_begin_index, block_define.m_actual_range.m_end_index,
			m_run->measure(size_range.m_begin_index, size_range.m_end_index, in_scale, in_text_context), in_text_context);//todo:add renderer
	}
}