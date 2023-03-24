#include <PreCompileHeader.h>

#include "SComplexGradient.h"

#include "SlateCore/Rendering/DrawElementPayloads.h"//FSlateGradientStop depends on it
#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

namespace DoDo
{
	SComplexGradient::SComplexGradient()
		: m_gradient_colors(*this)
		, m_desired_size_override(*this)
	{
		//todo:add set can tick
	}

	void SComplexGradient::Construct(const FArguments& in_args)
	{
		m_desired_size_override.Assign(*this, in_args._DesiredSizeOverride);
		m_gradient_colors.Assign(*this, in_args._GradientColors);
		m_b_has_alpha_back_ground = in_args._HasAlphaBackground;
		m_orientation = in_args._Orientation;
	}

	int32_t SComplexGradient::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//todo:add color picker, alpha back ground
		ESlateDrawEffect draw_effects = (b_parent_enabled && Is_Enabled()) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		const std::vector<FLinearColor>& colors = m_gradient_colors.Get();
		int32_t num_colors = colors.size();

		if(num_colors > 0)
		{
			std::vector<FSlateGradientStop> gradient_stops;

			for(int32_t color_index = 0; color_index < num_colors; ++color_index)
			{
				gradient_stops.push_back(FSlateGradientStop(allotted_geometry.get_local_size() * (float(color_index) / (num_colors - 1)), colors[color_index]));
			}

			FSlateDrawElement::make_gradient(out_draw_elements,
				layer_id + 1,
				allotted_geometry.to_paint_geometry(),
				gradient_stops,
				m_orientation,
				draw_effects
			);
		}

		return layer_id + 1;
	}

	glm::vec2 SComplexGradient::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		const std::optional<glm::vec2>& current_size_override = m_desired_size_override.Get();

		if(current_size_override.has_value())
		{
			return current_size_override.value();
		}

		return SCompoundWidget::Compute_Desired_Size(Layout_Scale_Multiplier);
	}
}
