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

		FPaintArgs with_new_parent(const SWidget* paint_parent) const
		{
			FPaintArgs args(*this);
			args.m_paint_parent_ptr = paint_parent;

			return args;
		}

		//todo:implement InsertCustomHitTestPath

		void set_inherited_hittest_ability(bool in_herited_hittest_ability)
		{
			m_b_inherited_hittest_ability = in_herited_hittest_ability;
		}

		const SWidget* get_paint_parent() const
		{
			return m_paint_parent_ptr;
		}

		FHittestGrid& get_hittest_grid() const
		{
			return m_current_grid;
		}

		double get_current_time() const
		{
			return m_current_time;
		}

		float get_delta_time() const
		{
			return m_delta_time;
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
