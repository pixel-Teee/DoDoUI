#include <PreCompileHeader.h>

#include "DrawElements.h"

#include "DrawElementPayloads.h"//FSlateBoxPayload
#include "SlateCore/Layout/PaintGeometry.h"//paint geometry
#include "SlateCore/Styling/SlateBrush.h"

#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo depends on it

#include "SlateCore/Widgets/SWidget.h"

#include "SlateCore/Fonts/FontCache.h"//FShapedGlyphSequence depends on it

namespace DoDo
{
	static bool should_cull(const FSlateWindowElementList& element_list, const FPaintGeometry& paint_geometry, const FSlateBrush* in_brush, const FLinearColor& in_tint)
	{
		bool b_is_full_transparent = in_tint.A == 0 && (in_brush->m_outline_settings.m_color.get_specified_color().A == 0 || in_brush->m_outline_settings.m_b_use_brush_transparency);

		if (b_is_full_transparent)//todo:add more cull
		{
			return true;
		}

		return false;
	}

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
		const FLinearColor& in_tint)
	{
		//todo:implement this function

		//call PaintGeometry's commit transforms if using legacy constructor
		paint_geometry.commit_transforms_if_using_legacy_constructor();
		//todo:implement should cull

		if (should_cull(element_list, paint_geometry, in_brush, in_tint))
		{
			return;
		}

		//todo:implement make box internal
		MakeBoxInternal(element_list, in_layer, paint_geometry, in_brush, in_draw_effects, in_tint);
	}

	void FSlateDrawElement::make_text(FSlateWindowElementList& element_list, uint32_t in_layer,
		const FPaintGeometry& paint_geometry, const DoDoUtf8String& in_text, const FSlateFontInfo& in_font_info,
		ESlateDrawEffect in_draw_effects, const FLinearColor& in_tint)
	{
		paint_geometry.commit_transforms_if_using_legacy_constructor();

		//don't try and render empty text
		if(in_text.get_length() == 0 || in_text == "")
		{
			return;
		}

		//don't do anything if there the font would be completely transparent
		if(in_tint.A == 0) //todo:implement outline settings
		{
			return;
		}

		FSlateDrawElement& element = element_list.add_uninitialized();

		FSlateTextPayload& data_pay_load = element_list.create_pay_load<FSlateTextPayload>(element);

		data_pay_load.set_tint(in_tint);
		data_pay_load.set_text(in_text, in_font_info);

		element.init(element_list, EElementType::ET_Text, in_layer, paint_geometry, in_draw_effects);
	}

	void FSlateDrawElement::make_shaped_text(FSlateWindowElementList& element_list, int32_t in_layer, const FPaintGeometry& paint_geometry, const FShapedGlyphSequencePtr& in_shaped_glyph_sequence, ESlateDrawEffect in_draw_effects, const FLinearColor& base_tint, const FLinearColor& outline_tint, FTextOverflowArgs text_overflow_args)
	{
		paint_geometry.commit_transforms_if_using_legacy_constructor();

		if (in_shaped_glyph_sequence->get_glyphs_to_render().size() == 0)
		{
			return;
		}

		//todo:add should cull

		//don't do anything if there the font would be completely transparent
		if ((base_tint.A == 0 && in_shaped_glyph_sequence->get_font_outline_settings().m_outline_size == 0) || (base_tint.A == 0 && outline_tint.A == 0))
		{
			return;
		}

		FSlateDrawElement& element = element_list.add_uninitialized();

		FSlateShapedTextPayload& data_pay_load = element_list.create_pay_load<FSlateShapedTextPayload>(element);
		data_pay_load.set_tint(base_tint);
		data_pay_load.set_shaped_text( in_shaped_glyph_sequence, outline_tint);
		data_pay_load.set_overflow_args(text_overflow_args);

		element.init(element_list, EElementType::ET_ShapedText, in_layer, paint_geometry, in_draw_effects);
	}

	void FSlateDrawElement::make_gradient(FSlateWindowElementList& element_list, uint32_t in_layer,
		const FPaintGeometry& paint_geometry, std::vector<FSlateGradientStop> in_gradient_stops,
		EOrientation in_gradient_type, ESlateDrawEffect in_draw_effects, glm::vec4 corner_radius)
	{
		paint_geometry.commit_transforms_if_using_legacy_constructor();

		FSlateDrawElement& element = element_list.add_uninitialized();

		FSlateGradientPayload& data_pay_load = element_list.create_pay_load<FSlateGradientPayload>(element);

		data_pay_load.set_gradient(in_gradient_stops, in_gradient_type, corner_radius);

		element.init(element_list, EElementType::ET_Gradient, in_layer, paint_geometry, in_draw_effects);
	}

	void FSlateDrawElement::make_viewport(FSlateWindowElementList& element_list, uint32_t in_layer, const FPaintGeometry& paint_geometry, std::shared_ptr<const ISlateViewport> viewport, ESlateDrawEffect in_draw_effects, const FLinearColor& in_tint)
	{
		paint_geometry.commit_transforms_if_using_legacy_constructor();

		//todo:add cull

		FSlateDrawElement& element = element_list.add_uninitialized();
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
	                                                      const FLinearColor& in_tint)
	{
		//todo:implement EElementType
		//EElementType element_type = (in_brush->m_draw_as == ESlateBrushDrawType::Border) ? EElementType::ET_Border : (in_brush->m_draw_as == ESlateBrushDrawType::RoundedBox) ? EElementType::ET_RoundedBox : EElementType::ET_Box;

		EElementType element_type = (in_brush->m_draw_as == ESlateBrushDrawType::Border) ? EElementType::ET_Border : (in_brush->m_draw_as == ESlateBrushDrawType::RoundedBox) ? EElementType::ET_RoundedBox :
			EElementType::ET_Box;//note 

		//todo:create a FSlateDrawElement

		//cast to rounded rect to get the internal parameters
		//new payload type - inherit from box pay load
		FSlateDrawElement& element = element_list.add_uninitialized();

		FSlateBoxPayload* box_payload;

		if(element_type == EElementType::ET_RoundedBox)
		{
			//crate rounded box pay load
			FSlateRoundedBoxPayload* r_box_pay_load = &element_list.create_pay_load<FSlateRoundedBoxPayload>(element);

			glm::vec4 corner_radii = in_brush->m_outline_settings.m_corner_radii;

			if (in_brush->m_outline_settings.m_rounding_type == ESlateBrushRoundingType::HalfHeightRadius)
			{
				const float uniform_radius = paint_geometry.get_local_Size().y / 2.0f;
				corner_radii = glm::vec4(uniform_radius, uniform_radius, uniform_radius, uniform_radius);
			}
			r_box_pay_load->set_radius(corner_radii);

			if (in_brush->m_outline_settings.m_b_use_brush_transparency)
			{
				//glm::vec4 color = in_brush->m_outline_settings.m_color;//todo:copy with new opacity
				FLinearColor color = in_brush->m_outline_settings.m_color.get_specified_color().copy_with_new_opacity(in_tint.A);
				r_box_pay_load->set_outline(color, in_brush->m_outline_settings.m_width);
			}
			else
			{
				r_box_pay_load->set_outline(in_brush->m_outline_settings.m_color.get_specified_color(), in_brush->m_outline_settings.m_width);
			}
			box_payload = r_box_pay_load;
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
		, m_b_needs_deferred_resolve(false)
	{
		//todo:check in_paint_window is valid
	}

	FSlateWindowElementList::~FSlateWindowElementList()
	{
		
	}

	void FSlateWindowElementList::queue_deferred_painting(const FDeferredPaint& in_deferred_paint)
	{
		m_deferred_paint_list.push_back(std::make_shared<FDeferredPaint>(in_deferred_paint));
	}

	int32_t FSlateWindowElementList::paint_deferred(int32_t layer_id, const FSlateRect& my_culling_rect)
	{
		//todo:
		m_b_needs_deferred_resolve = false;

		int32_t resolve_index = m_resolve_to_deferred_index.back();

		m_resolve_to_deferred_index.pop_back();

		for (int32_t i = resolve_index; i < m_deferred_paint_list.size(); ++i)
		{
			layer_id = m_deferred_paint_list[i]->execute_paint(layer_id, *this, my_culling_rect);
		}

		for (int32_t i = m_deferred_paint_list.size() - 1; i >= resolve_index; --i)
		{
			m_deferred_paint_list.erase(m_deferred_paint_list.begin() + i);
		}

		return layer_id;
	}

	void FSlateWindowElementList::begin_deferred_group()
	{
		m_resolve_to_deferred_index.push_back(m_deferred_paint_list.size());
	}

	void FSlateWindowElementList::end_deferred_group()
	{
		m_b_needs_deferred_resolve = true;
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
	FSlateWindowElementList::FDeferredPaint::FDeferredPaint(const std::shared_ptr<const SWidget>& in_widget_to_paint, const FPaintArgs& in_args, const FGeometry& in_allotted_geometry, const FWidgetStyle& in_widget_style, bool in_parent_enabled)
		: m_widget_to_paint_ptr(in_widget_to_paint)
		, m_args(in_args)
		, m_allotted_geometry(in_allotted_geometry)
		, m_widget_style(in_widget_style)
		, m_b_parent_enabled(in_parent_enabled)
	{
		//todo:FPaintArgs add set deferred paint
	}
	int32_t FSlateWindowElementList::FDeferredPaint::execute_paint(int32_t layer_id, FSlateWindowElementList& out_draw_elements, const FSlateRect& my_culling_rect)
	{
		std::shared_ptr<const SWidget> widget_to_paint = m_widget_to_paint_ptr.lock();
		if (widget_to_paint)
		{
			return widget_to_paint->paint(m_args, m_allotted_geometry, my_culling_rect, out_draw_elements, layer_id, m_widget_style, m_b_parent_enabled);
		}

		return layer_id;
	}
}
