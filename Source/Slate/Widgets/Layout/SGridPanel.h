#pragma once

#include "SlateCore/Widgets/SPanel.h"

namespace DoDo {
	class SGridPanel : public SPanel
	{
		SLATE_DECLARE_WIDGET(SGridPanel, SPanel)

	public:


	public:
		//swidget interface
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual void cache_desired_size(float in_layout_scale_multiplier) override;

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;

		virtual FChildren* Get_Children() override;
	};
}