#pragma once

#include "SWidget.h"

namespace DoDo
{
	class SLeafWidget : public SWidget
	{
	public:
		SLeafWidget()
		{
			b_can_have_children = false;
		}

		//todo:implement set visibility
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect,
			FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override = 0;

		virtual glm::vec2 Compute_Desired_Size(float) const override = 0;

		virtual FChildren* Get_Children() override;

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;
	private:
		//todo:implement these functions
	};
}
