#include "PreCompileHeader.h"

#include "SPopup.h"

#include "SlateCore/Layout/ArrangedChildren.h"

#include "SlateCore/Rendering/DrawElements.h"

namespace DoDo {
	void SPopup::Construct(const FArguments& in_args)
	{
		this->m_child_slot
		[
			in_args._Content.m_widget
		];
	}
	int32_t SPopup::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		FArrangedChildren arranged_children(EVisibility::visible);
		this->Arrange_Children(allotted_geometry, arranged_children);

		//there may be zero elements in this array if our child collapsed/hidden
		if (arranged_children.num() > 0)
		{
			const bool b_should_be_enabled = should_be_enabled(b_parent_enabled);

			FArrangedWidget& the_child = arranged_children[0];

			FWidgetStyle compounded_widget_style = FWidgetStyle(in_widget_style)
				.blend_color_and_opacity_tint(get_color_and_opacity())
				.set_foreground_color(b_should_be_enabled ? get_foreground_color() : get_disabled_foreground_color());

			//an SPopup just queues up its children to be painted after everything in this window is done painting
			out_draw_elements.queue_deferred_painting(
				FSlateWindowElementList::FDeferredPaint(the_child.m_widget, args.with_new_parent(this), the_child.m_geometry, compounded_widget_style, b_should_be_enabled)
			);
		}
		return layer_id;
	}
}