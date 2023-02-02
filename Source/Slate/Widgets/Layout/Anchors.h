#pragma once

#include "glm/vec2.hpp"

namespace DoDo
{
	/*
	 * describes how a widget is anchored
	 */
	struct FAnchors
	{
		/*holds the minimum anchors, left + top*/
		glm::vec2 m_minimum;

		/*holds the maximum anchors, right + bottom*/
		glm::vec2 m_maximum;

	public:
		/*
		 * default constructor
		 *
		 * the default margin size is zero on all four sides
		 */
		FAnchors()
			: m_minimum(0)
			, m_maximum(0)
		{}

		FAnchors(float min_x, float min_y, float max_x, float max_y)
			: m_minimum(min_x, min_y)
			, m_maximum(max_x, max_y)
		{
			
		}
	};
}
