#pragma once

#include "glm/glm.hpp"

#include "SlateRect.h"

namespace DoDo
{
	/*
	 * stores a rectangle that has been transformed by an arbitrary render transform
	 * we provide a ctor that does the work common to slate drawing, but you could technically
	 * create this any way you want
	 */
	struct FSlateRotatedRect
	{
	public:
		/*default ctor*/
		FSlateRotatedRect() {}

		/*construct a rotated rect from a given aligned rect*/
		explicit FSlateRotatedRect(const FSlateRect& aligned_rect)
			: m_top_left(aligned_rect.left, aligned_rect.top)
			, m_extent_x(aligned_rect.right - aligned_rect.left, 0.0f)
			, m_extent_y(0.0f, aligned_rect.bottom - aligned_rect.top)
		{}

		/*per-element constructor*/
		FSlateRotatedRect(const glm::vec2& in_top_left, const glm::vec2& in_extent_x, const glm::vec2& in_extent_y)
			: m_top_left(in_top_left)
			, m_extent_x(in_extent_x)
			, m_extent_y(in_extent_y)
		{}

	public:
		/*transformed top-left corner*/
		glm::vec2 m_top_left;
		/*transformed x extent(right-left)*/
		glm::vec2 m_extent_x;
		/*transformed y extent(bottom-top)*/
		glm::vec2 m_extent_y;

		/*convert to a bounding, aligned rect*/
		FSlateRect to_bounding_rect() const;

	public:
		/*point-in-rect test*/
		bool is_under_location(const glm::vec2& location) const;
	};

	/*transforms a rect by the given transform*/
	template<typename TransformType>
	FSlateRotatedRect transform_rect(const TransformType& transform, const FSlateRotatedRect& rect)
	{
		return FSlateRotatedRect(
			transform_point(transform, rect.m_top_left),//in top left
			transform_vector(transform, rect.m_extent_x),
			transform_vector(transform, rect.m_extent_y)
		);//transform FSlateRotatedRect, but don't effect the length(extent)
	}
}