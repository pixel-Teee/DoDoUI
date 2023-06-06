#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	class ISlateViewport;

	class SViewport : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SViewport)
			: _Content()
			, _ShowEffectWhenDisabled(true)
			, _ViewportSize(get_default_viewport_size())
		{}

			SLATE_DEFAULT_SLOT(FArguments, Content)

			/*whether or not to show the disabled effect when this viewport is disabled*/
			SLATE_ATTRIBUTE(bool, ShowEffectWhenDisabled)

			/*the interface to be used by this viewport for rendering and I/O*/
			SLATE_ARGUMENT(std::shared_ptr<ISlateViewport>, ViewportInterface)

			/*size of the viewport widget*/
			SLATE_ATTRIBUTE(glm::vec2, ViewportSize)

			static glm::vec2 get_default_viewport_size();
		SLATE_END_ARGS()

		/*default constructor*/
		SViewport();

		/*
		* construct the widget
		* 
		* @param InArgs declaration from which to construct the widget
		*/
		void Construct(const FArguments& in_args);

		/*
		* computes the ideal size necessary to display this widget
		*/
		virtual glm::vec2 Compute_Desired_Size(float) const override
		{
			return m_viewport_size.Get();
		}

		/*
		* sets the interface to be used by this viewport for rendering and I/O
		* 
		* @param InViewportInterface the interface to use
		*/
		void set_viewport_interface(std::shared_ptr<ISlateViewport> in_viewport_interface);
	public:
		//------SWidget interface------
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		//------SWidget interface------
	protected:
		/*interface to the rendering and I/O implementation of the viewport*/
		std::weak_ptr<ISlateViewport> m_viewport_interface;

	private:
		/*whether or not to show the disabled effect when this viewport is disabled*/
		TSlateAttribute<bool, EInvalidateWidgetReason::Paint> m_show_disabled_effect;

		/*size of the viewport*/
		TSlateAttribute<glm::vec2, EInvalidateWidgetReason::Layout> m_viewport_size;
	};
}