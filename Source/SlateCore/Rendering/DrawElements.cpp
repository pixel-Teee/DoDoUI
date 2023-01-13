#include <PreCompileHeader.h>

#include "DrawElements.h"

#include "SlateCore/Layout/PaintGeometry.h"//paint geometry

namespace DoDo
{
	void FSlateDrawElement::MakeBox(
		FSlateWindowElementList& element_list,
		uint32_t in_layer,
		const FPaintGeometry& paint_geometry,
		const FSlateBrush* in_brush,
		ESlateDrawEffect in_draw_effects,
		const glm::vec4& in_tint)
	{
		//todo:implement this function

		//call PaintGeometry's commit transforms if using legacy constructor

		//todo:implement should cull

		//todo:implement make box internal
	}

	FSlateDrawElement& FSlateDrawElement::MakeBoxInternal(FSlateWindowElementList& element_list, uint32_t in_layer,
		const FPaintGeometry& paint_geometry, const FSlateBrush* in_brush, ESlateDrawEffect in_draw_effect,
		const glm::vec4& in_tint)
	{
		//todo:implement EElementType

		//todo:create a FSlateDrawElement


	}
}
