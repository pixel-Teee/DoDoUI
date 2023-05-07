#pragma once

#include "Core/Core.h"

#include "SlateCore/Styling/SlateTypes.h"//ETextOverflowPolicy depends on it

#include "Core/Internationlization/Text.h"//TextBiDi depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

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

	class FTextLayout : public std::enable_shared_from_this<FTextLayout>
	{
	public:

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

			//todo:add FRunModel FBreakCandidate, FTextRunRenderer, FTextLineHighlight

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
	protected:

		/*
		* calculates the visual justification for the given line view
		*/
		ETextJustify::Type calculate_line_view_visual_justification(const FLineView& line_view) const;

		/*the views for the lines of text, a line view represents a single visual line of text, multiple line views can map to the same line model, if for example wrapping occurs*/
		std::vector<FLineView> m_line_views;

		/*override for the text overflow policy, if unset, the style is used*/
		std::optional<ETextOverflowPolicy> m_text_overflow_policy_override;

		/*how the text should be aligned with the margin*/
		ETextJustify::Type m_justification;
	};
}