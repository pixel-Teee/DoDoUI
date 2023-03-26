#include <PreCompileHeader.h>

#include "SBorder.h"

#include "SlateCore/Layout/FlowDirection.h"//use g_slate_flow_direction
#include "SlateCore/Layout/Geometry.h"//MakeBox's FGeometry depends on it
#include "SlateCore/Rendering/DrawElements.h"//on paint depends on it
#include "SlateCore/Styling/SlateBrush.h"//FSlateBrush depends on it

#include "SlateCore/Styling/SlateWidgetStyle.h"
#include "SlateCore/Styling/CoreStyle.h"

#include "SlateCore/Styling/ISlateStyle.h"

namespace DoDo
{
	void SBorder::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this function
	}

	SBorder::SBorder()
		: m_border_image_attribute(*this, FCoreStyle::get().get_brush("Border"))//todo:implement FCoreStyle
		, m_desired_size_scale_attribute(*this, glm::vec2(1.0f, 1.0f))
		, m_show_disabled_effect_attribute(*this, true)
		, m_b_flip_for_right_to_left_flow_direction(false)
		, m_border_back_ground_color_attribute(*this)
	{
	}

	void SBorder::Construct(const FArguments& in_args)
	{
		//set content scale
		Set_Content_Scale(in_args._ContentScale);

		set_color_and_opacity(in_args._ColorAndOpacity);

		set_border_back_ground_color(in_args._BorderBackgroundColor);

		set_border_image(in_args._BorderImage);

		if(in_args._OnMouseMove.is_bound())
		{
			set_on_mouse_move(in_args._OnMouseMove);
		}

		m_child_slot
		.HAlign(in_args._HAlign)
		.VAlign(in_args._VAlign)
		.Padding(in_args._Padding)
		[
			in_args._Content.m_widget
		];
	}

	void SBorder::set_border_back_ground_color(TAttribute<FSlateColor> in_color_and_opacity)
	{
		m_border_back_ground_color_attribute.Assign(*this, std::move(in_color_and_opacity));
	}

	void SBorder::set_padding(TAttribute<FMargin> in_padding)
	{
		m_child_slot.set_padding(in_padding);
	}

	void SBorder::set_border_image(TAttribute<const FSlateBrush*> in_border_image)
	{
		//todo:may be need fix
		m_border_image_attribute.Assign(*this, in_border_image);
	}

	//int32_t i = 0;
	//
	//void SBorder::set_color()
	//{
	//	if(i % 2)
	//	{
	//		m_border_back_ground_color_attribute.Set(*this, glm::vec4(0.2f, 0.6f, 0.9f, 1.0f));
	//		++i;
	//	}
	//	else
	//	{
	//		m_border_back_ground_color_attribute.Set(*this, glm::vec4(0.8f, 0.1f, 0.3f, 1.0f));
	//		++i;
	//	}
	//}

	void SBorder::set_content(std::shared_ptr<SWidget> in_content)
	{
		m_child_slot
		[
			in_content
		];
	}

	int32_t SBorder::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
	                          const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
	                          const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//todo:get the FSlateBrush
		//todo:fix me, this is nullptr
		const FSlateBrush* brush_resource = m_border_image_attribute.Get();//todo:to implement BorderImageAttribute

		const bool b_enabled = should_be_enabled(b_parent_enabled);//put function parameter to this function

		const ESlateDrawEffect draw_effects = ESlateDrawEffect::None;

		//todo:implement ESlateDrawEffect
		//todo:implement FSlateBrush

		if (brush_resource && brush_resource->m_draw_as != ESlateBrushDrawType::NoDrawType)
		{
			if (m_b_flip_for_right_to_left_flow_direction && g_slate_flow_direction == EFlowDirection::RightToLeft)
			{
				//todo:implement make child
				//const FGeometry flipped_geometry = allotted_geometry.make_child()

				//todo:implement FSlateDrawElement MakeBox

				//todo:implement this function

				FSlateDrawElement::MakeBox(
					out_draw_elements,
					layer_id,
					allotted_geometry.to_paint_geometry(),
					brush_resource,
					draw_effects,
					brush_resource->get_tint(in_widget_style) * in_widget_style.get_color_and_opacity_tint() * m_border_back_ground_color_attribute.Get().get_color(in_widget_style)//todo:to implement FSlateBrush's function get tint function
				);

			}
			else
			{
                 FSlateDrawElement::MakeBox(
					out_draw_elements,
					layer_id,
					allotted_geometry.to_paint_geometry(),
					brush_resource,
					draw_effects,
					brush_resource->get_tint(in_widget_style) * in_widget_style.get_color_and_opacity_tint() * m_border_back_ground_color_attribute.Get().get_color(in_widget_style)//todo:to implement FSlateBrush's function get tint function
				);
			}
		}

		//note:brush resource's image color * brush resource's color * border back ground color
		//note:widget style will affect the child widgets

		return SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id,
		                                 in_widget_style,
		                                 b_parent_enabled);
	}
}
