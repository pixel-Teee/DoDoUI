#pragma once

#include <Core/Core.h>

#include "SlateCore/Layout/SlateRect.h"//FSlateRect

namespace DoDo
{
	class FWidgetStyle;
	class FPaintArgs;
	class FSlateWindowElementList;

	//context class
	struct FSlateInvalidationContext
	{
		FSlateInvalidationContext(FSlateWindowElementList& in_window_element_list, const FWidgetStyle& in_widget_style)
			: m_paint_args(nullptr)
			, m_widget_style(in_widget_style)
			, m_window_element_list(&in_window_element_list)
			, m_layout_scale_multiplier(1.0f)
			, m_in_coming_layer_id(0)
			, m_b_parent_enabled(true)
			, m_b_allow_fast_path_update(false)
		{
			
		}

		FSlateRect m_culling_rect;
		const FPaintArgs* m_paint_args;
		const FWidgetStyle& m_widget_style;
		FSlateWindowElementList* m_window_element_list;
		float m_layout_scale_multiplier;
		int32_t m_in_coming_layer_id;
		bool m_b_parent_enabled;
		bool m_b_allow_fast_path_update;
	};

	struct FSlateInvalidationResult
	{
		FSlateInvalidationResult()
			: m_max_layer_id_painted(0)
			, m_b_repainted_widgets(false)
		{}

		/*the max layer id painted or cached*/
		int32_t m_max_layer_id_painted;

		/*if we had to repaint any widget*/
		bool m_b_repainted_widgets;
	};

	class FSlateInvalidationRoot
	{
	public:
		FSlateInvalidationRoot();

		virtual ~FSlateInvalidationRoot();

		/*@return the cached elements for this window and it's widget hierarchy*/
		FSlateInvalidationResult paint_invalidation_root(const FSlateInvalidationContext& context);

		virtual int32_t paint_slow_path(const FSlateInvalidationContext& context) = 0;

	private:
		int32_t m_cached_max_layer_id;
	};
}
