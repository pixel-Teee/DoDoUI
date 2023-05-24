#include "PreCompileHeader.h"

#include "SlateTextBlockLayout.h"

#include "Slate/Framework/Text/SlateTextLayout.h"//FSlateTextLayout depends on it

#include "Slate/Widgets/Text/PlainTextLayoutMarshaller.h"//ITextLayoutMarshaller depends on it

namespace DoDo {
	FSlateTextBlockLayout::FSlateTextBlockLayout()
	{
	}
	FSlateTextBlockLayout::FSlateTextBlockLayout(SWidget* in_owner, FTextBlockStyle in_default_text_style, const std::optional<ETextShapingMethod> in_text_shaping_method, const std::optional<ETextFlowDirection> in_text_flow_direction, std::shared_ptr<ITextLayoutMarshaller> in_marshaller)
		: m_text_layout(FSlateTextLayout::Create(in_owner, std::move(in_default_text_style)))
		, m_marshaller(std::move(in_marshaller))
		, m_cache_wrap_text_at(0)
		, m_cached_auto_wrap_text(false)
	{
		//todo:add set text shaping method

		//todo:add set text flow direction

		//todo:add set line break iterator
	}
	glm::vec2 FSlateTextBlockLayout::get_desired_size() const
	{
		return m_text_layout->get_size();
	}
	int32_t FSlateTextBlockLayout::On_Paint(const FPaintArgs& in_paint_args, const FGeometry& in_allotted_geometry, const FSlateRect& in_clipping_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled)
	{
		return m_text_layout->On_Paint(in_paint_args, in_allotted_geometry, in_clipping_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);
	}
	glm::vec2 FSlateTextBlockLayout::Compute_Desired_Size(const FWidgetDesiredSizeArgs& in_widget_args, const float in_scale, const FTextBlockStyle& in_text_style)
	{
		//cache the wrapping rules so that we can recompute the wrap at width in paint
		m_cache_wrap_text_at = in_widget_args.m_wrap_text_at;
		m_cached_auto_wrap_text = in_widget_args.m_auto_wrap_text;

		const ETextTransformPolicy previous_transform_policy = ETextTransformPolicy::None;//todo:fix me

		//set the layout information
		m_text_layout->set_scale(in_scale);
		//todo:set wrapping width

		{
			bool b_requires_text_update = true;
			const DoDoUtf8String& text_to_set = in_widget_args.m_text;
			
			//todo:add FTextSnapShot

			if (b_requires_text_update)
			{
				update_text_layout(text_to_set);
			}
		}

		//we need to update our size if the text layout has become dirty
		m_text_layout->update_if_needed();

		return m_text_layout->get_size();
	}
	void FSlateTextBlockLayout::update_text_layout(const DoDoUtf8String& in_text)
	{
		//update_text_layout(in_text);
		
		//todo:clear

		m_marshaller->set_text(in_text, *m_text_layout); //note:populate the line model and line view
	}
}