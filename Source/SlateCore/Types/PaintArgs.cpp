#include <PreCompileHeader.h>

#include "PaintArgs.h"

namespace DoDo
{
	FPaintArgs::FPaintArgs(const SWidget* paint_parent, FHittestGrid& in_root_hittest_grid,
		FHittestGrid& in_current_hittest_grid, glm::vec2 in_window_offset, double in_current_time, float in_delta_time)
			: m_root_grid(in_root_hittest_grid)
			, m_current_grid(in_current_hittest_grid)
			, m_window_offset(in_window_offset)
			, m_paint_parent_ptr(paint_parent)
			, m_current_time(in_current_time)
			, m_delta_time(in_delta_time)
			, m_b_inherited_hittest_ability(true)
			, m_b_deferred_painting(false)
	{
	}

	FPaintArgs::FPaintArgs(const SWidget* paint_parent, FHittestGrid& in_root_hittest_grid, glm::vec2 in_window_offset,
		double in_current_time, float in_delta_time)
	: FPaintArgs(paint_parent, in_root_hittest_grid, in_root_hittest_grid, in_window_offset, in_current_time, in_delta_time)
	{
	}
}
