#include "PreCompileHeader.h"

#include "TextLayout.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

#include "Slate/Framework/Text/ShapedTextCache.h"

#include "Application/Application.h"

#include "Renderer/Renderer.h"

namespace DoDo {
	FTextLayout::~FTextLayout()
	{
	}
	glm::vec2 FTextLayout::get_size() const
	{
		//todo:add FTextLayoutSize
		return glm::vec2(1.0f, 1.0f);
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

	FRunModel::FRunModel(const std::shared_ptr<IRun>& in_run)
		: m_run(in_run)
	{
	}
	std::shared_ptr<IRun> FRunModel::get_run() const
	{
		return m_run;
	}
}