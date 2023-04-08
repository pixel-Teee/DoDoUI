#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"//SCompoundWidget depend on it

//#include "Core/Math/Color.h"
#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	/*
	* implements a slate widget that renders a simple color gradient
	*/
	class SSimpleGradient : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SSimpleGradient)
			: _StartColor(FLinearColor(0.0f, 0.0f, 0.0f))
			, _EndColor(FLinearColor(1.0f, 1.0f, 1.0f))
			, _Orientation(Orient_Vertical)
		{}
			/*the leftmost gradient color*/
			SLATE_ATTRIBUTE(FLinearColor, StartColor)

			/*the rightmost gradient color*/
			SLATE_ATTRIBUTE(FLinearColor, EndColor)

			/*horizontal or vertical gradient*/
			SLATE_ATTRIBUTE(EOrientation, Orientation)
		SLATE_END_ARGS()

	public:
		SSimpleGradient();

		/*
		* constructs the widget
		* 
		* @param InArgs the declaration data for this widget
		*/
		void Construct(const FArguments& in_args);

	protected:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

	protected:
		/*the leftmost gradient color*/
		TSlateAttribute<FLinearColor, EInvalidateWidgetReason::Paint> m_start_color;

		/*the rightmost gradient color*/
		TSlateAttribute<FLinearColor, EInvalidateWidgetReason::Paint> m_end_color;

		/*horizontal or vertical gradient*/
		EOrientation m_orientation;
	};
}