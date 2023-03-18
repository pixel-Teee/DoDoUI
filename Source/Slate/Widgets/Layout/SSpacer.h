#pragma once

#include "SlateCore/Widgets/SLeafWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	class SSpacer : public SLeafWidget
	{
		SLATE_DECLARE_WIDGET(SSpacer, SLeafWidget)

	public:
		SLATE_BEGIN_ARGS(SSpacer)
		{}
			SLATE_ATTRIBUTE(glm::vec2, Size)
		SLATE_END_ARGS()

		SSpacer();

		/*
		* construct this widget
		* 
		* @param InArgs the declaration data for this widget
		*/
		void Construct(const FArguments& in_args);

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		glm::vec2 get_size() const
		{
			if (m_b_is_spacer_size_bound)
			{
				SSpacer& mutable_self = const_cast<SSpacer&>(*this);
				mutable_self.m_spacer_size.update_now(mutable_self);
			}
			return m_spacer_size.Get();
		}

		void set_size(TAttribute<glm::vec2> in_spacer_size)
		{
			m_b_is_spacer_size_bound = in_spacer_size.Is_Bound();
			m_spacer_size.Assign(*this, in_spacer_size);
		}

	protected:
		virtual glm::vec2 Compute_Desired_Size(float) const override;
	private:
		TSlateAttribute<glm::vec2> m_spacer_size;
		bool m_b_is_spacer_size_bound;
	};
}