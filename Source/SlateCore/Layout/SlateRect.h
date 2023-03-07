#pragma once

#include "glm/glm.hpp"

namespace DoDo
{
	/*
	 * a rectangle defined by upper-left and lower-left corners
	 *
	 * assumes a "screen-like" coordinate system where the origin is in the top-left, with the y-axis going down
	 * functions like "contains" etc will not work with other conventions
	 */
	class FSlateRect
	{
	public:
		float left;
		float top;
		float right;
		float bottom;

		explicit FSlateRect(float in_left = -1, float in_top = - 1, float in_right = -1, float in_bottom = -1)
			: left(in_left)
			, top(in_top)
			, right(in_right)
			, bottom(in_bottom)
		{}

		FSlateRect(const glm::vec2& in_start_pos, const glm::vec2& in_end_pos)
			: left(in_start_pos.x)
			, top(in_start_pos.y)
			, right(in_end_pos.x)
			, bottom(in_end_pos.y)
		{}

		/*
		 * returns the top-left position of the rectangle
		 * @return the top-left position
		 */
		glm::vec2 get_top_left() const
		{
			return glm::vec2(left, top);
		}

		glm::vec2 get_bottom_right() const
		{
			return glm::vec2(right, bottom);
		}

		glm::vec2 get_size() const
		{
			return glm::vec2(right - left, bottom - top);
		}

	private:

	};

	template<typename TransformType>
	FSlateRect transform_rect(const TransformType& transform, const FSlateRect& rect)
	{
		glm::vec2 top_left_transformed = transform_point(transform, glm::vec2(rect.left, rect.top));
		glm::vec2 bottom_right_transformed = transform_point(transform, glm::vec2(rect.right, rect.bottom));

		if (top_left_transformed.x > bottom_right_transformed.x)
		{
			std::swap(top_left_transformed.x, bottom_right_transformed.x);
		}

		if (top_left_transformed.y > bottom_right_transformed.y)
		{
			std::swap(top_left_transformed.y, bottom_right_transformed.y);
		}

		return FSlateRect(top_left_transformed, bottom_right_transformed);
	}
}