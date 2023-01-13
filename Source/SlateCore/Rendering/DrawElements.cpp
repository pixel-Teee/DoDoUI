#include <PreCompileHeader.h>

#include "DrawElements.h"

#include "SlateCore/Layout/PaintGeometry.h"//paint geometry
#include "SlateCore/Styling/SlateBrush.h"

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
		EElementType element_type = (in_brush->m_draw_as == ESlateBrushDrawType::Border) ? EElementType::ET_Border : (in_brush->m_draw_as == ESlateBrushDrawType::RoundedBox) ? EElementType::ET_RoundedBox : EElementType::ET_Box;

		//todo:create a FSlateDrawElement

		//cast to rounded rect to get the internal parameters
		//new payload type - inherit from box pay load
		FSlateDrawElement& element = element_list.add_uninitialized();



	}

	FSlateDrawElement& FSlateWindowElementList::add_uninitialized()
	{
		const bool b_allow_cache = false;//todo:fix this

		//todo:implement if else branch

		FSlateDrawElementArray& elements = m_uncached_draw_elements;

		//add default element
		elements.push_back(FSlateDrawElement());

		const int32_t insert_index = elements.size() - 1;

		//get the last element
		FSlateDrawElement& new_element = elements[insert_index];
		return elements[insert_index];
	}
}
