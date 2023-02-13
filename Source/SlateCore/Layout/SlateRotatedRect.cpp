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

	static float cross(const glm::vec2& lhs, const glm::vec2& rhs)
	{
		return lhs.x * rhs.y - rhs.x * lhs.y;//cross dot
	}

	bool FSlateRotatedRect::is_under_location(const glm::vec2& location) const
	{
		//note:check point is in this rect
		const glm::vec2 offset = location - m_top_left;

		//const float det = glm::cross(m_extent_x, m_extent_y);
		const float det = cross(m_extent_x, m_extent_y);

		//not exhaustively efficient, could optimize the checks for [0..1] to short circuit faster
		const float s = -cross(offset, m_extent_x) / det;

		if(s >= 0.0f && s <= 1.0f)
		{
			const float t = cross(offset, m_extent_y) / det;
			return t >= 0.0f && t <= 1.0f;
		}
		return false;
	}
}
