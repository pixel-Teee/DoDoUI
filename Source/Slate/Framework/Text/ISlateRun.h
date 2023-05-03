#pragma once

#include "Core/Core.h"

#include "Slate/Framework/Text/TextLayout.h"//FTextArgs depends on it

#include "SlateCore/Rendering/DrawElementPayloads.h"//ETextOverflowDirection depends on it

#include "IRun.h"//IRun

namespace DoDo {
	//class FTextLayout;
	//struct FTextLayout::FLineView;
	struct FTextArgs
	{
		FTextArgs(const FTextLayout::FLineView& in_line, const std::shared_ptr<ILayoutBlock>& in_block, const FTextBlockStyle& in_default_style, ETextOverflowPolicy in_overflow_policy,
			ETextOverflowDirection in_overflow_direction)
			: m_line(in_line)
			, m_block(in_block)
			, m_default_style(in_default_style)
			, m_overflow_policy(in_overflow_policy)
			, m_overflow_direction(in_overflow_direction)
			, m_b_force_ellipsis_due_to_clipped_line(false)
		{}
		const FTextLayout::FLineView& m_line;
		const std::shared_ptr<ILayoutBlock>& m_block;
		const FTextBlockStyle& m_default_style;
		ETextOverflowPolicy m_overflow_policy;
		ETextOverflowDirection m_overflow_direction;
		bool m_b_force_ellipsis_due_to_clipped_line;
	};

	class ISlateRun : public IRun
	{
	public:
		//virtual int32_t On_Paint(const FPi)
	};
}