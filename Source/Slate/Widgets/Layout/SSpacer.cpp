#include <PreCompileHeader.h>

#include "SSpacer.h"

namespace DoDo {
	void SSpacer::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this function
	}

	SSpacer::SSpacer()
		: m_spacer_size(*this, glm::vec2(0.0f))
		, m_b_is_spacer_size_bound(false)
	{
		//todo:add set can tick and can support focus
	}
	void SSpacer::Construct(const FArguments& in_args)
	{
		set_size(in_args._Size);
	}
	int32_t SSpacer::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		return layer_id;
	}
	glm::vec2 SSpacer::Compute_Desired_Size(float) const
	{
		return m_spacer_size.Get();
	}
}