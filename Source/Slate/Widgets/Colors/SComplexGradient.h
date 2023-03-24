#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

namespace DoDo
{
	/*
	 * implements a slate widget that renders a color gradient consisting of multiple stops
	 */
	class SComplexGradient : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SComplexGradient)
			: _GradientColors()
			, _HasAlphaBackground(false)
			, _Orientation(Orient_Vertical)
		{
			//todo:add visibility
		}

			/*the colors used in the gradient*/
		SLATE_ATTRIBUTE(std::vector<FLinearColor>, GradientColors)

			/*whether a checker background is displayed for alpha viewing*/
			SLATE_ARGUMENT(bool, HasAlphaBackground)

			/*horizontal or vertical gradient*/
			SLATE_ARGUMENT(EOrientation, Orientation)

			/*when specified use this as the gradients desired size*/
			SLATE_ATTRIBUTE(std::optional<glm::vec2>, DesiredSizeOverride)
		SLATE_END_ARGS()

		SComplexGradient();

		/*
		 * construct this widget
		 *
		 * @param InArgs the declaration data for this widget
		 */
		void Construct(const FArguments& in_args);
	protected:

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, 
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, 
		int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;
	private:

		/*the colors used in the gradient*/
		TSlateAttribute<std::vector<FLinearColor>, EInvalidateWidgetReason::Paint> m_gradient_colors;

		/*optional override for desired size*/
		TSlateAttribute<std::optional<glm::vec2>, EInvalidateWidgetReason::Layout> m_desired_size_override;

		/*whether a checker background is displayed for alpha viewing*/
		bool m_b_has_alpha_back_ground;

		/*horizontal or vertical gradient*/
		EOrientation m_orientation;
	};
}
