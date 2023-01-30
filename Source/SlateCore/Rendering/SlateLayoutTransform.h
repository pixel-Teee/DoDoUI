#pragma once

#include "glm/vec2.hpp"

namespace DoDo {
	/*
		represents a 2d transformation in the following order
		scale then translate used by FGeometry for it's layout transformations

		Matrix form looks like:
		[Vx Vy 1] * [ S  0  0 ]
					[ 0  S  0 ]
					[ Tx Ty 1 ]
	*/
	class FSlateLayoutTransform
	{
	public:
		//ctor from a scale followed by translate, shortcut to concatenate(InScale, InTranslation)
		explicit FSlateLayoutTransform(float in_scale = 1.0f, const glm::vec2& in_translation = glm::vec2(0.0f, 0.f));

		/*access to the 2d translation*/
		glm::vec2 get_translation() const
		{
			return m_translation;
		}

		/*access to the scale*/
		float get_scale() const
		{
			return m_scale;
		}

	private:
		float m_scale;//scale
		glm::vec2 m_translation;
	};
}
