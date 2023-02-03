#include <PreCompileHeader.h>

#include "SPanel.h"

#include "SlateCore/Layout/ArrangedChildren.h"
#include "SlateCore/Types/PaintArgs.h"

namespace DoDo
{
	int32_t SPanel::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		FArrangedChildren arranged_children(EVisibility::visible);

		//this function will to call virtual function
		Arrange_Children(allotted_geometry, arranged_children);//todo:call virtual function to arrange children

		//todo:call other function
		return paint_arranged_children(args, arranged_children, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);
	}

	int32_t SPanel::paint_arranged_children(const FPaintArgs& args, const FArrangedChildren& arranged_children,
		const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect,
		FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style,
		bool b_parent_enabled) const
	{
		//because we paint multiple children, we must track the maximum layer id that they produced in case one of our parents
		//wants to an overlay for all of it's contents
		int32_t max_layer_id = layer_id;

		//todo:fix FPaintArgs
		//const FPaintArgs new_args = args.with_new_parent(this);
		const bool b_should_be_enabled = should_be_enabled(b_parent_enabled);

		for(int32_t child_index = 0; child_index < arranged_children.num(); ++child_index)
		{
			const FArrangedWidget& current_widget = arranged_children[child_index];

			//todo:implement culling

			//dispatch to virtual function
			const int32_t curr_widgets_max_layer_id = current_widget.m_widget->paint(args, current_widget.m_geometry, my_culling_rect, out_draw_elements, layer_id,
				in_widget_style, b_parent_enabled);
		}

		return max_layer_id;
	}
}
