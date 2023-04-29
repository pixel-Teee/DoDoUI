#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

namespace DoDo {
	/*a popup's contents show up on top of other things*/
	class SPopup : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SPopup)
		{
			_Visibility = EVisibility::SelfHitTestInvisible;
		}
			SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_END_ARGS()

		SPopup()
		{

		}

		void Construct(const FArguments& in_args);

	private:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
	};
}