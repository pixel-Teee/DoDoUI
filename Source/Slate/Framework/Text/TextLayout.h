#pragma once

#include "Core/Core.h"

#include "SlateCore/Styling/SlateTypes.h"//ETextOverflowPolicy depends on it

#include "Core/Internationlization/Text.h"//TextBiDi depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

#include "ShapedTextCacheFwd.h"

namespace DoDo {
	class ILayoutBlock;

	namespace ETextJustify
	{
		enum Type
		{
			/*
			* justify the text logically to the left,
			* when text is flowing left-to-right, this will align text visually to the left.
			* when text is flowing right-to-left, this will align text visually to the right.
			*/
			Left,

			/*
			* justify the text in the center.
			* text flow direction has no impact on this justification mode.
			*/
			Center,

			/*
			* justify the text logically to the right.
			* when text is flowing left-to-right, this will align text visually to the right.
			* when text is flowing right-to-left, this will align text visually to the left
			*/
			Right
		};
	}

	enum class ETextFlowDirection : uint8_t
	{
		Auto = 0,

		LeftToRight,

		RightToLeft
	};

	enum class ETextWrappingPolicy : uint8_t
	{
		DefaultWrapping = 0,

		AllowPerCharacterWrapping
	};

	class IRun;
	struct FLayoutBlockTextContext;
	class FTextLayout : public std::enable_shared_from_this<FTextLayout>
	{
	public:

		struct FBlockDefinition
		{
			/*range inclusive of trailing whitespace, as used to visually display and interact with the text*/
			FTextRange m_actual_range;

			/*the render to use with this run*/
			//todo:add
		};

		class FRunModel
		{
		public:
			FRunModel(const std::shared_ptr<IRun>& in_run);

			std::shared_ptr<IRun> get_run() const;

			FTextRange get_text_range() const;

			std::shared_ptr<ILayoutBlock> create_block(const FTextLayout::FBlockDefinition& block_define, float in_scale, const FLayoutBlockTextContext& in_text_context) const;
		private:
			std::shared_ptr<IRun> m_run;
			std::vector<FTextRange> m_measured_ranges;
			std::vector<glm::vec2> m_measured_range_sizes;
		};

		virtual ~FTextLayout();

		glm::vec2 get_size() const;

		float get_scale() const { return m_scale; }
		void set_scale(float value);

		//const FTextBlockStyle& get_default_text_style() const;

		struct FNewLineData
		{
			FNewLineData(std::shared_ptr<DoDoUtf8String> in_text, std::vector<std::shared_ptr<IRun>> in_runs)
				: m_text(in_text)
				, m_runs(std::move(in_runs))
			{}
			std::shared_ptr<DoDoUtf8String> m_text;
			std::vector<std::shared_ptr<IRun>> m_runs;
		};

		void add_lines(const std::vector<FNewLineData>& new_lines);
 
		struct ELineModelDirtyState
		{
			typedef uint8_t Flags;
			enum Enum
			{
				None = 0,
				WrappingInformation = 1 << 0,
				TextBaseDirection = 1 << 1,
				ShapingCache = 1 << 2,
				All = WrappingInformation | TextBaseDirection | ShapingCache
			};
		};

		struct FLineModel
		{
		public:
			FLineModel(const std::shared_ptr<DoDoUtf8String>& in_text);

			std::shared_ptr<DoDoUtf8String> m_text;
			//todo:add FShapedTextCacheRef and TextBiDi

			FShapedTextCachePtr m_shaped_text_cache;

			TextBiDi::ETextDirection m_text_base_direction;

			//std::vector<FRunModel> 
			//todo:add FRunModel FBreakCandidate, FTextRunRenderer, FTextLineHighlight
			std::vector<FRunModel> m_runs;
		};

		struct FLineViewHighLight
		{
			/*offset in x for this highlight, relative to the FLineView::Offset that contains it*/
			float m_offset_x;
			/*width for this highlight, the height will be either FLineView::Size.Y or FLineView::TextHeight depending on whether you want to highlight the entire line, or just the text within the line*/
			float m_width;

			//todo:add ILineHighlighter
		};

		struct FLineView
		{
			std::vector<std::shared_ptr<ILayoutBlock>> m_blocks;
			std::vector<FLineViewHighLight> m_underlay_high_lights;
			std::vector<FLineViewHighLight> m_overlay_high_lights;
			glm::vec2 m_offset;
			glm::vec2 m_size;
			float m_text_height;
			float m_justification_width;
			FTextRange m_range;
			//todo:implement TextBiDi
			TextBiDi::ETextDirection m_text_base_direction;
			int32_t m_model_index;
		};

		const std::vector<FTextLayout::FLineModel>& get_line_models() const { return m_line_models; }
	private:

		void flow_layout();

		void flow_line_layout(const int32_t line_model_index, const float wrapping_draw_width, std::vector<std::shared_ptr<ILayoutBlock>>& soft_line);

		void create_line_view_blocks(int32_t line_model_index, const int32_t stop_index, const float wrapped_line_width,
			const std::optional<float>& justification_width, int32_t& out_run_index, int32_t& out_renderer_index, int32_t& out_previous_block_end, std::vector<std::shared_ptr<ILayoutBlock>>& out_soft_line);

	protected:

		/*
		* calculates the visual justification for the given line view
		*/
		ETextJustify::Type calculate_line_view_visual_justification(const FLineView& line_view) const;

		/*the models for the lines of text, a line model represents a single string with no manual breaks*/
		std::vector<FLineModel> m_line_models;

		/*the views for the lines of text, a line view represents a single visual line of text, multiple line views can map to the same line model, if for example wrapping occurs*/
		std::vector<FLineView> m_line_views;

		/*override for the text overflow policy, if unset, the style is used*/
		std::optional<ETextOverflowPolicy> m_text_overflow_policy_override;

		/*the scale to draw the text at*/
		float m_scale;

		/*how the text should be aligned with the margin*/
		ETextJustify::Type m_justification;
	};
}