#pragma once

#include "Core/Core.h"

#include "SlateCore/Styling/SlateTypes.h"//ETextOverflowPolicy depends on it

#include "Core/Internationlization/Text.h"//TextBiDi depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

#include "ShapedTextCacheFwd.h"

namespace DoDo {
	class ILayoutBlock;
	class IBreakIterator;

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

	/*
	* the different directions that text can flow within a paragraph of text
	* @note if you change this enum, make sure and update CVarDefaultTextFlowDirection and GetDefaultTextFlowDirection
	*/
	enum class ETextFlowDirection : uint8_t
	{
		Auto = 0,

		LeftToRight,

		RightToLeft
	};

	/*
	* the different methods that can be used if a word is too long to be broken by the default line-break iterator
	*/
	enum class ETextWrappingPolicy : uint8_t
	{
		/*no fallback, just use the given line-break iterator*/
		DefaultWrapping = 0,

		/*fallback to per-character wrapping a word is too long*/
		AllowPerCharacterWrapping
	};

	/*location within the text model*/
	struct FTextLocation
	{
	public:
		FTextLocation(const int32_t in_line_index = 0, const int32_t in_offset = 0)
			: m_line_index(in_line_index)
			, m_offset(in_offset)
		{}

		FTextLocation(const FTextLocation& in_location, const int32_t in_offset)
			: m_line_index(in_location.get_line_index())
			, m_offset(std::max(in_location.get_offset() + in_offset, 0))
		{}

		bool operator==(const FTextLocation& other) const
		{
			return m_line_index == other.m_line_index && m_offset == other.m_offset;
		}

		bool operator!=(const FTextLocation& other) const
		{
			return m_line_index != other.m_line_index && m_offset != other.m_offset;
		}

		bool operator<(const FTextLocation& other) const
		{
			return this->m_line_index < other.m_line_index || (this->m_line_index == other.m_line_index && this->m_offset < other.m_offset);
		}

		int32_t get_line_index() const { return m_line_index; }
		int32_t get_offset() const { return m_offset; }
		bool is_valid() const { return m_line_index != -1 && m_offset != -1; }
	private:
		int32_t m_line_index;
		int32_t m_offset;
	};

	class IRun;
	struct FLayoutBlockTextContext;
	struct FTextLocation;
	class FTextLayout : public std::enable_shared_from_this<FTextLayout>
	{
	public:
		FTextLayout();

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

			void set_text_range(const FTextRange& value);

			int16_t get_base_line(float scale) const;

			int16_t get_max_height(float scale) const;

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

		float get_wrapping_width() const { return m_wrapping_width; }
		void set_wrapping_width(float value);

		ETextTransformPolicy get_transform_policy() const { return m_transform_policy; }
		void set_transform_policy(ETextTransformPolicy value);

		//const FTextBlockStyle& get_default_text_style() const;

		void clear_lines();

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

		bool insert_at(const FTextLocation& location, DoDoUtf8String character);

		/*
		* updates the TextLayout's if any changes have occurred since the last update
		*/
		virtual void update_if_needed();

		virtual void update_layout();
 
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

			FShapedTextCachePtr m_shaped_text_cache;//note:this is important, FLineModel holds the life time of this

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
	protected:
		/*
		* clears the current layout view information
		*/
		void clear_view();
	private:
		float get_wrapping_draw_width() const;

		void flow_layout();

		void flow_line_layout(const int32_t line_model_index, const float wrapping_draw_width, std::vector<std::shared_ptr<ILayoutBlock>>& soft_line);

		void create_line_view_blocks(int32_t line_model_index, const int32_t stop_index, const float wrapped_line_width,
			const std::optional<float>& justification_width, int32_t& out_run_index, int32_t& out_renderer_index, int32_t& out_previous_block_end, std::vector<std::shared_ptr<ILayoutBlock>>& out_soft_line);

	protected:
		struct FTextLayoutSize
		{
			FTextLayoutSize()
				: m_draw_width(0.0f)
				, m_wrapped_width(0.0f)
				, m_height(0.0f)
			{}

			glm::vec2 get_draw_size() const
			{
				return glm::vec2(m_draw_width, m_height);
			}

			glm::vec2 get_wrapped_size() const
			{
				return glm::vec2(m_wrapped_width, m_height);
			}

			/*width of text layout, including any lines which extend beyond the wrapping boundaries (eg, lines with lots of trailing whitespace, or lines with no break candidates)*/
			float m_draw_width;

			/*width of text layout after the text has been wrapped, and including the first piece of trailing whitespace for any given soft-wrapped line*/
			float m_wrapped_width;

			/*height of the text layout*/
			float m_height;
		};

		/*
		* calculates the visual justification for the given line view
		*/
		ETextJustify::Type calculate_line_view_visual_justification(const FLineView& line_view) const;

		/*the models for the lines of text, a line model represents a single string with no manual breaks*/
		std::vector<FLineModel> m_line_models;

		/*the views for the lines of text, a line view represents a single visual line of text, multiple line views can map to the same line model, if for example wrapping occurs*/
		std::vector<FLineView> m_line_views;

		ETextWrappingPolicy m_wrapping_policy;

		ETextTransformPolicy m_transform_policy;

		/*override for the text overflow policy, if unset, the style is used*/
		std::optional<ETextOverflowPolicy> m_text_overflow_policy_override;

		/*the scale to draw the text at*/
		float m_scale;

		/*the width that the text should be wrap at, if 0 or negative no wrapping occurs*/
		float m_wrapping_width;

		/*how the text should be aligned with the margin*/
		ETextJustify::Type m_justification;

		/*the iterator to use to detect graphme cluster boundaries*/
		std::shared_ptr<IBreakIterator> m_grapheme_break_iterator;


		/*the final size of the text layout on screen*/
		FTextLayoutSize m_text_layout_size;
	};
}