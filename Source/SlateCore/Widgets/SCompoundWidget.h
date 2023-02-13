#pragma once

#include "SWidget.h"

//#include "glm/glm.hpp"

#include "SlateCore/Layout/Children.h"//SCompoundWidget depends on it

namespace DoDo {
	class FPaintArgs;
	struct FGeometry;
	class FSlateRect;
	class FSlateWindowElementList;
	class FWidgetStyle;
	/*
	 * a compound widget is the base from which most non-primitive widgets should be built
	 * compound widgets have a protected member named child slot
	 */
	class SCompoundWidget : public SWidget
	{
		SLATE_DECLARE_WIDGET(SCompoundWidget, SWidget)
	public:
		//because TSlateAttribute's delete constructor, so SCompound Widget's default constructor could not use
		//SCompoundWidget();

		virtual ~SCompoundWidget();

		/*
			returns the size scaling factor for this widget
			return size scaling factor
		*/
		const glm::vec2 Get_Content_Scale() const
		{
			return m_content_scale_attribute.Get();
		}

		/*
			set the content scale for this widget
			param InCotentScale Content scaling factor
		 */
		void Set_Content_Scale(TAttribute<glm::vec2> In_Content_Scale)
		{
			m_content_scale_attribute.Assign(*this, std::move(In_Content_Scale));
		}

		/*
		* sets the widget's foreground color
		*
		* @param InColor the color to set
		*/
		void set_foreground_color(TAttribute<glm::vec4> in_fore_ground_color)
		{
			m_foreground_color_attribute.Assign(*this, std::move(in_fore_ground_color));
		}

		//todo:implement FPaintArgs
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual FChildren* Get_Children() override;

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

	protected:

		/*disallow public construction*/
		SCompoundWidget();

		/*------begin SWidget overrides------*/
		virtual glm::vec2 Compute_Desired_Size(float) const override;
		/*------end SWidget overrides------*/

		struct FCompoundWidgetOneChildSlot : TSingleWidgetChildrenWithBasicLayoutSlot<EInvalidateWidgetReason::None>
		{
			friend SCompoundWidget;
			using TSingleWidgetChildrenWithBasicLayoutSlot<EInvalidateWidgetReason::None>::TSingleWidgetChildrenWithBasicLayoutSlot;
		};

		/* the slot that contains this widget's descendants */
		FCompoundWidgetOneChildSlot m_child_slot;

	private:

		//the layout scale to apply to this widget's contents, useful for animation
		TSlateAttribute<glm::vec2> m_content_scale_attribute;

		//the color and opacity to apply to this widget and all its descendants
		//todo:need to use linear color
		TSlateAttribute<glm::vec4> m_color_and_opacity_attribute;

		//optional foreground color that will be inherited by all of this widget's contents
		TSlateAttribute<glm::vec4> m_foreground_color_attribute;
	};
}