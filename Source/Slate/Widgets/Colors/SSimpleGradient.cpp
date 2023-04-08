#include "PreCompileHeader.h"

#include "SSimpleGradient.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect

#include "SlateCore/Rendering/DrawElementPayloads.h"//FSlateGradientStop

#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

namespace DoDo {
	SSimpleGradient::SSimpleGradient()
		: m_start_color(*this, FLinearColor(0.0f, 0.0f, 0.0f))
		, m_end_color(*this, FLinearColor(1.0f, 1.0f, 1.0f))
	{

	}

	void SSimpleGradient::Construct(const FArguments& in_args)
	{
		m_start_color.Assign(*this, in_args._StartColor);
		m_end_color.Assign(*this, in_args._EndColor);

		m_orientation = in_args._Orientation.Get();
	}

	int32_t SSimpleGradient::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const ESlateDrawEffect draw_effects = this->should_be_enabled(b_parent_enabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		std::vector<FSlateGradientStop> gradient_stop;

		gradient_stop.push_back(FSlateGradientStop(glm::vec2(0.0f), m_start_color.Get()));
		gradient_stop.push_back(FSlateGradientStop(allotted_geometry.get_local_size(), m_end_color.Get()));

		FSlateDrawElement::make_gradient(out_draw_elements,
			layer_id + 1,
			allotted_geometry.to_paint_geometry(),
			gradient_stop,
			m_orientation,
			draw_effects | ESlateDrawEffect::NoGamma);

		return layer_id + 1;
	}
}