#include <PreCompileHeader.h>

#include "STextBlock.h"

#include "Application/Application.h"//Application

#include "Renderer/Renderer.h"//compute desired function depends on it

#include "SlateCore/Fonts/FontMeasure.h"//compute desired depends on it

#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

#include "Slate/Widgets/Text/PlainTextLayoutMarshaller.h"//PlainTextLayoutMarshaller depends on it

namespace DoDo
{
	void STextBlock::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//todo:to register attribute
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "Font", m_bound_text, EInvalidateWidgetReason::Layout);//todo:fix me, use slate attribute member attribute definition
	}

	STextBlock::STextBlock()
		: m_bound_text(*this)
		, m_font(*this)
		, m_strike_brush(*this)
		, m_color_and_opacity(*this)
		, m_shadow_offset(*this, glm::vec2(0.0f))
		, m_shadow_color_and_opacity(*this, glm::vec4(1.0f))
		, m_highlight_color(*this, glm::vec4(1.0f))//todo:implement force init
		, m_high_light_shape(*this)
		, m_highlight_text(*this)
		, m_wrap_text_at(*this, 0.0f)
		, m_auto_wrap_text(*this, false)
		, m_wrapping_policy(*this, ETextWrappingPolicy::DefaultWrapping)
		, m_transform_policy(*this)
		, m_margin(*this)
		, m_justification(*this)
		, m_line_height_percentage(*this, 1.0f)
		, m_min_desired_width(*this, 0.0f)
		, m_b_simple_text_mode(false)//todo:modify this to false
	{


	}

	STextBlock::~STextBlock()
	{
	}

	void STextBlock::Construct(const FArguments& in_args)
	{
		m_text_style = *in_args._TextStyle;

		set_text(in_args._Text);

		set_font(in_args._Font);

		set_color_and_opacity(in_args._ColorAndOpacity);

		m_text_layout_cache = std::make_unique<FSlateTextBlockLayout>(this, FTextBlockStyle::get_default(), in_args._TextShapingMethod, in_args._TextFlowDirection, FPlainTextLayoutMarshaller::Create());
	}

	FSlateColor STextBlock::get_color_and_opacity() const
	{
		return m_b_is_attribute_color_and_opacity_set ? m_color_and_opacity.Get() : m_text_style.m_color_and_opacity;
	}

	int32_t STextBlock::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{

		if(m_b_simple_text_mode)
		{
			//draw the optional shadow

			const bool b_should_be_enabled = should_be_enabled(b_parent_enabled);

			//todo:draw shadow

			//draw the text itself
			const DoDoUtf8String& local_text = m_bound_text.Get();
			FSlateFontInfo local_font = get_font();

			FSlateDrawElement::make_text(out_draw_elements,
				layer_id,
				allotted_geometry.to_paint_geometry(),
				local_text,//todo:fix me, use bound text's get function
				local_font,
				b_should_be_enabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
				in_widget_style.get_color_and_opacity_tint() * get_color_and_opacity().get_color(in_widget_style));//todo:fix color
		}
		else
		{
			//todo:user text layout cache
			const glm::vec2 last_desired_size = glm::vec2(20.0f, 20.0f);

			//OnPaint will also update the text layout cache if required
			layer_id = m_text_layout_cache->On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, should_be_enabled(b_parent_enabled));

			const glm::vec2 new_desired_size = m_text_layout_cache->get_desired_size();

			//todo:add wrap logic
		}

		return layer_id;
	}

	glm::vec2 STextBlock::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		if(m_b_simple_text_mode)
		{
			//todo:calculate local shadow offset and local outline size

			const glm::vec2 text_size = Application::get().get_renderer()->get_font_measure_service()->measure(m_bound_text.Get(), get_font());

			m_cached_simple_desired_size = glm::vec2(std::max(m_min_desired_width.Get(), text_size.x), text_size.y);

			return m_cached_simple_desired_size.value();//get the value
		}
		else
		{
			//compute desired size will also update the layout cache if required
			const glm::vec2 text_size = m_text_layout_cache->Compute_Desired_Size(
				FSlateTextBlockLayout::FWidgetDesiredSizeArgs(
					m_bound_text.Get(),
					m_highlight_text.Get(),
					m_wrap_text_at.Get(),
					m_auto_wrap_text.Get(),
					m_wrapping_policy.Get(),
					m_transform_policy.Get(),
					m_margin.Get(),
					m_line_height_percentage.Get(),
					m_justification.Get()
				),
				Layout_Scale_Multiplier,
				m_text_style //todo:modify this as get computed text style
			);

			return glm::vec2(20.0f, 20.0f);//todo:add min desired width
		}
	}

	void STextBlock::set_text(TAttribute<DoDoUtf8String> in_text)
	{
		//cache the IsBound
		//when the attribute is not bound, we need to go through all the other bound property to check if it is bound
		m_b_is_attribute_bound_text_bound = in_text.Is_Bound();
		m_bound_text.Assign(*this, std::move(in_text));
	}

	void STextBlock::set_font(TAttribute<FSlateFontInfo> in_font)
	{
		m_b_is_attribute_font_set = in_font.Is_Set();
		m_font.Assign(*this, std::move(in_font));
	}

	void STextBlock::set_color_and_opacity(TAttribute<FSlateColor> in_color_and_opacity)
	{
		m_b_is_attribute_color_and_opacity_set = in_color_and_opacity.Is_Set();
		m_color_and_opacity.Assign(*this, std::move(in_color_and_opacity));
	}

	FSlateFontInfo STextBlock::get_font() const
	{
		return m_b_is_attribute_font_set ? m_font.Get() : m_text_style.m_font;
	}
}
