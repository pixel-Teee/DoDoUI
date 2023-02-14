#include <PreCompileHeader.h>

#include "STextBlock.h"

namespace DoDo
{
	void STextBlock::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//todo:to register attribute
	}

	STextBlock::STextBlock()
		: m_bound_text(*this)
		, m_strike_brush(*this)
		, m_color_and_opacity(*this)
		, m_shadow_offset(*this, glm::vec2(0.0f))
		, m_shadow_color_and_opacity(*this, glm::vec4(1.0f))
		, m_highlight_color(*this, glm::vec4(1.0f))//todo:implement force init
		, m_high_light_shape(*this)
		, m_highlight_text(*this)
		, m_wrap_text_at(*this, 0.0f)
		, m_auto_wrap_text(*this, false)
		, m_margin(*this)
		, m_line_height_percentage(*this, 1.0f)
		, m_min_desired_width(*this, 0.0f)
		, m_b_simple_text_mode(false)
	{
	}

	STextBlock::~STextBlock()
	{
	}

	void STextBlock::Construct(const FArguments& in_args)
	{

	}

	int32_t STextBlock::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		if(m_b_simple_text_mode)
		{
			//draw the optional shadow

			//todo:draw shadow

			//draw the text itself
		}
		else
		{
			//todo:user text layout cache
		}

		return layer_id;
	}

	glm::vec2 STextBlock::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		if(m_b_simple_text_mode)
		{
			return glm::vec2(0.0f, 0.0f);//todo:implement this function
		}
		else
		{
			return glm::vec2(0.0f, 0.0f);
		}
	}
}
