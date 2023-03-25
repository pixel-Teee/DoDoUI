#include <PreCompileHeader.h>

#include "SGridPanel.h"

namespace DoDo {
	void SGridPanel::Private_Register_Attributes(FSlateAttributeInitializer&)
	{

	}
	int32_t SGridPanel::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		return 0;
	}
	void SGridPanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
	}
	void SGridPanel::cache_desired_size(float in_layout_scale_multiplier)
	{
	}
	glm::vec2 SGridPanel::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		return glm::vec2();
	}
	FChildren* SGridPanel::Get_Children()
	{
		return nullptr;
	}
}