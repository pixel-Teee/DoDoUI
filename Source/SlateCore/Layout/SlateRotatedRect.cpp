#include <PreCompileHeader.h>

#include "SlateRotatedRect.h"

namespace DoDo
{
	FSlateRect FSlateRotatedRect::to_bounding_rect() const
	{
		glm::vec2 points[4] = {
			m_top_left,
			m_top_left + m_extent_x,
			m_top_left + m_extent_y,
			m_top_left + m_extent_x + m_extent_y
		};

		return FSlateRect(
			std::min(points[0].x, std::min(std::min(points[1].x, points[2].x), points[3].x)),
			std::min(points[0].y, std::min(std::min(points[1].y, points[2].y), points[3].y)),
			std::max(points[0].x, std::max(std::max(points[1].x, points[2].x), points[3].x)),
			std::max(points[0].y, std::max(std::max(points[1].y, points[2].y), points[3].y))
		);
	}
}
