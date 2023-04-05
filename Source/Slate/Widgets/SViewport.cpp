#include "PreCompileHeader.h"

#include "SViewport.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depends on it

#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

namespace DoDo {
	glm::vec2 SViewport::FArguments::get_default_viewport_size()
	{
		return glm::vec2(320.0f, 240.0f);
	}

	SViewport::SViewport()
		: m_show_disabled_effect(*this, true)
		, m_viewport_size(*this, SViewport::FArguments::get_default_viewport_size())
	{
	}

	void SViewport::Construct(const FArguments& in_args)
	{
		m_show_disabled_effect.Assign(*this, in_args._ShowEffectWhenDisabled);
		m_viewport_interface = in_args._ViewportInterface;
		m_viewport_size.Assign(*this, in_args._ViewportSize);

		this->m_child_slot
		[
			in_args._Content.m_widget
		];
	}

	void SViewport::set_viewport_interface(std::shared_ptr<ISlateViewport> in_viewport_interface)
	{
		if (m_viewport_interface.lock() != in_viewport_interface)
		{
			m_viewport_interface = in_viewport_interface;
			Invalidate(EInvalidateWidgetReason::Paint);//mark
		}
	}

	int32_t SViewport::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		bool b_enabled = should_be_enabled(b_parent_enabled);
		
		bool b_show_disabled_effect = m_show_disabled_effect.Get();
		ESlateDrawEffect draw_effects = b_show_disabled_effect && !b_enabled ? ESlateDrawEffect::DisabledEffect : ESlateDrawEffect::None;

		//todo:add more effects

		std::shared_ptr<ISlateViewport> viewport_interface_lock = m_viewport_interface.lock();

		//tell the interface that we are drawing
		if (viewport_interface_lock)
		{
			//note:resize functionality
			//todo:implement this function
			//viewport_interface->on_draw_viewport()
		}

		//todo:check render to backbuffer directly
		if (viewport_interface_lock)
		{
			FSlateDrawElement::make_viewport(out_draw_elements, layer_id, allotted_geometry.to_paint_geometry(), viewport_interface_lock, draw_effects, in_widget_style.get_color_and_opacity_tint());
		}

		int32_t layer = SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_enabled);

		return layer;
	}
}