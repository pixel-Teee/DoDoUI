#pragma once

#include "SlateCore/Styling/SlateTypes.h"//FTextBlockStyle depends on it

#include "SlateCore/Fonts/FontCache.h"//ETextShapingMethod

#include "Slate/Framework/Text/TextLayout.h"

namespace DoDo {
	class SWidget;
	class FSlateTextLayout;
	class FPaintArgs;
	struct FGeometry;
	class FSlateRect;
	class FSlateWindowElementList;
	/*class to handle the cached layout of STextBlock by proxying around a FTextLayout*/
	class FSlateTextBlockLayout
	{
	public:
		FSlateTextBlockLayout();

		/*
		* constructor
		*/
		FSlateTextBlockLayout(SWidget* in_owner, FTextBlockStyle in_default_text_style,
			const std::optional<ETextShapingMethod> in_text_shaping_method);

		glm::vec2 get_desired_size() const;

		/*
		* paint this layout, updating the internal cache as required
		*/
		int32_t On_Paint(const FPaintArgs& in_paint_args, const FGeometry& in_allotted_geometry, const FSlateRect& in_clipping_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style,
			bool b_parent_enabled);
	private:
		/*in control of the layout and wrapping of the text*/
		std::shared_ptr<FSlateTextLayout> m_text_layout;
	};
}