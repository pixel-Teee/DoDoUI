#pragma once

#include "SNodePanel.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	class SGraphPanel : public SNodePanel
	{
	public:
		SLATE_BEGIN_ARGS(SGraphPanel)
		{}
		SLATE_END_ARGS()
		/*
		* construct a widget
		* 
		* @param InArgs the declaration describing how the widgets should be constructed
		*/
		void Construct(const FArguments& in_args);

		~SGraphPanel();

	public:
		//SWidget interface
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
		//End of SWidget interface

	protected:
		/*geometry cache*/
		mutable glm::vec2 m_cached_allotted_geometry_scaled_size;
	};
}