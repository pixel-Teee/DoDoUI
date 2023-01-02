#pragma once

#include "glm/vec2.hpp"

namespace DoDo
{
	class FHittestGrid;
	class SWidget;
	/*
	 * SWidget::OnPaint and SWidget::Paint use FParintArgs as their
	 * sole parameter in order to ease the burden of passing
	 * through multiple fields
	 */
	class FPaintArgs
	{
	public:
		FPaintArgs(const SWidget* paint_parent, FHittestGrid& in_root_hittest_grid, FHittestGrid& in_current_hittest_grid, glm::vec2 in_window_offset, double in_current_time,
			float in_delta_time);

		FPaintArgs(const SWidget* paint_parent, FHittestGrid& in_root_hittest_grid, glm::vec2 in_window_offset, double in_current_time, float in_delta_time);

		//todo:implement InsertCustomHitTestPath

		void set_inherited_hittest_ability(bool in_herited_hittest_ability)
		{
			m_b_inherited_hittest_ability = in_herited_hittest_ability;
		}

		//todo:implement other set and get functions
	private:
		/*the root most grid, only the window should set this and only invalidation panels should modify it*/
		FHittestGrid& m_root_grid;

		/*the current hit test grid, it possible that there is more that one grid when there is nested invalidation panels, this is what widgets should add to always*/
		FHittestGrid& m_current_grid;

		glm::vec2 m_window_offset;

		const SWidget* m_paint_parent_ptr;

		double m_current_time;

		float m_delta_time;

		uint8_t m_b_inherited_hittest_ability : 1;//what it is?

		uint8_t m_b_deferred_painting : 1;//what it is?
	};
}
