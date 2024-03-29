#include <PreCompileHeader.h>

#include "DrawElements.h"

#include "DrawElementPayloads.h"//FSlateBoxPayload
#include "SlateCore/Layout/PaintGeometry.h"//paint geometry
#include "SlateCore/Styling/SlateBrush.h"

namespace DoDo
{
	FSlateDrawElement::FSlateDrawElement()
		: m_data_payload(nullptr)
	{
	}

	FSlateDrawElement::~FSlateDrawElement()
	{
		if(m_data_payload)
		{
			//m_data_payload->~FSlateDataPayload();

			delete m_data_payload;

			m_data_payload = nullptr;
		}
	}

	FSlateDrawElement::FSlateDrawElement(FSlateDrawElement&& rhs)
	{
		if(rhs.m_data_payload)//move life time
		{
			m_data_payload = rhs.m_data_payload;
			rhs.m_data_payload = nullptr;
		}

		m_render_transform = rhs.m_render_transform;
		m_position = rhs.m_position;
		m_local_size = rhs.m_local_size;
		m_layer_id = rhs.m_layer_id;
		m_scale = rhs.m_scale;
		m_draw_effect = rhs.m_draw_effect;
		m_element_type = rhs.m_element_type;
	}

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
		paint_geometry.commit_transforms_if_using_legacy_constructor();
		//todo:implement should cull

		//todo:implement make box internal
		MakeBoxInternal(element_list, in_layer, paint_geometry, in_brush, in_draw_effects, in_tint);
	}

	void FSlateDrawElement::init(FSlateWindowElementList& element_list, EElementType in_element_type, uint32_t in_layer,
		const FPaintGeometry& paint_geometry, ESlateDrawEffect in_draw_effects)
	{
		m_render_transform = paint_geometry.get_accumulated_render_transform();
		m_position = paint_geometry.m_draw_position;
		m_scale = paint_geometry.m_draw_scale;
		m_local_size = paint_geometry.get_local_Size();//todo:why SWindow's local size is 0.0?

		m_layer_id = in_layer;//layer id

		m_element_type = in_element_type;
		m_draw_effect = in_draw_effects;

		//todo:implement batch flags
	}

	FSlateDrawElement& FSlateDrawElement::MakeBoxInternal(FSlateWindowElementList& element_list, uint32_t in_layer,
	                                                      const FPaintGeometry& paint_geometry, const FSlateBrush* in_brush, ESlateDrawEffect in_draw_effect,
	                                                      const glm::vec4& in_tint)
	{
		//todo:implement EElementType
		//EElementType element_type = (in_brush->m_draw_as == ESlateBrushDrawType::Border) ? EElementType::ET_Border : (in_brush->m_draw_as == ESlateBrushDrawType::RoundedBox) ? EElementType::ET_RoundedBox : EElementType::ET_Box;

		EElementType element_type = EElementType::ET_Box;

		//todo:create a FSlateDrawElement

		//cast to rounded rect to get the internal parameters
		//new payload type - inherit from box pay load
		FSlateDrawElement& element = element_list.add_uninitialized();

		FSlateBoxPayload* box_payload;

		if(element_type == EElementType::ET_RoundedBox)
		{
			//crate rounded box pay load
		}
		else
		{
			//todo:implement element list's create pay load
			//allocate memory
			box_payload = &element_list.create_pay_load<FSlateBoxPayload>(element);
		}

		box_payload->set_tint(in_tint);//set tint

		box_payload->set_brush(in_brush, paint_geometry.get_local_Size(), paint_geometry.m_draw_scale);

		//todo:implement FSlateDrawElement's Init function
		element.init(element_list, element_type, in_layer, paint_geometry, in_draw_effect);

		return element;
	}

	FSlateWindowElementList::FSlateWindowElementList(const std::shared_ptr<SWindow>& in_paint_window)
		: m_weak_paint_window(in_paint_window)
		, m_raw_paint_window(in_paint_window.get())
		, m_window_size(glm::vec2(0.0f, 0.0f))
		, m_render_target_window(nullptr)
	{
		//todo:check in_paint_window is valid
	}

	FSlateWindowElementList::~FSlateWindowElementList()
	{
		
	}

	FSlateDrawElement& FSlateWindowElementList::add_uninitialized()
	{
		const bool b_allow_cache = false;//todo:fix this

		//todo:implement if else branch

		FSlateDrawElementArray& elements = m_uncached_draw_elements;

		//add default element
		elements.emplace_back();

		const int32_t insert_index = elements.size() - 1;

		//get the last element
		FSlateDrawElement& new_element = elements[insert_index];
		return elements[insert_index];
	}

	void FSlateWindowElementList::reset_element_list()
	{
		m_batch_data.reset_data();

		m_uncached_draw_elements.clear();//todo:this is important, every frame need to clear
	}

	FSlateBatchData& FSlateWindowElementList::get_batch_data()
	{
		return m_batch_data;
	}
}
