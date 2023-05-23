#pragma once

#include "TextLayout.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

namespace DoDo {
	class FSlateTextLayout : public FTextLayout
	{
	public:

		static std::shared_ptr<FSlateTextLayout> Create(SWidget* in_owner, FTextBlockStyle in_default_text_style);

		FSlateTextLayout(SWidget* in_owner, FTextBlockStyle in_default_text_style);

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const;
	protected:
		/*default style used by the text layout*/
		FTextBlockStyle m_default_text_style;

		virtual void aggregate_children();

	private:
		TSlotlessChildren<SWidget> m_children;
	};

}