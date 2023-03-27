#include <PreCompileHeader.h>

#include "SGraphPanel.h"

namespace DoDo {
	void SGraphPanel::Construct(const FArguments& in_args)
	{
		m_cached_allotted_geometry_scaled_size = glm::vec2(160.0f, 160.0f);
	}
	SGraphPanel::~SGraphPanel()
	{
	}
	int32_t SGraphPanel::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		m_cached_allotted_geometry_scaled_size = allotted_geometry.get_local_size() * allotted_geometry.m_scale;

		//style used for objects that are the same between revisions
		FWidgetStyle faded_style = in_widget_style;

		faded_style.blend_color_and_opacity_tint(FLinearColor(0.45f, 0.45f, 0.45f, 0.45f));

		//first paint the background
		const FSlateBrush* default_background = nullptr;//todo:from FEditorStyle to get graph panel solid back ground brush


		return layer_id;
	}
}