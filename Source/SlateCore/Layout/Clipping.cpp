#include <PreCompileHeader.h>

#include "Children.h"

namespace DoDo {
	FSlateClippingZone::FSlateClippingZone(const FGeometry& bounding_geometry)
		: m_b_interset(true)
		, m_b_always_clip(false)
	{
		const FSlateRenderTransform& transform = bounding_geometry.get_accumulated_render_transform();
		const glm::vec2& local_size = bounding_geometry.get_local_size();

		initialize_from_arbitrary_points(
			transform.transform_point(glm::vec2(0.0f, 0.0f)),
			transform.transform_point(glm::vec2(local_size.x, 0)),
			transform.transform_point(glm::vec2(0.0f, local_size.y)),
			transform.transform_point(local_size));
	}
	void FSlateClippingZone::initialize_from_arbitrary_points(const glm::vec2& in_top_left, const glm::vec2& in_top_right, const glm::vec2& in_bottom_left, const glm::vec2& in_bottom_right)
	{
		m_b_is_axis_aligned = false;

		//clipping is in pixel space, accept a very high tolerance
		const float tolerance = 0.1f;

		//since this is a rectangle check to edges, if their points are equal they are aligned to the same axis and thus the whole rect is aligned
		if (std::fabs(in_top_left.x - in_bottom_left.x) <= tolerance)
		{
			if (std::fabs(in_bottom_left.y - in_bottom_right.y) <= tolerance)
			{
				m_b_is_axis_aligned = true;
			}
		}
		else if (std::fabs(in_top_left.y - in_bottom_left.y) <= tolerance)
		{
			if (std::fabs(in_bottom_left.x - in_bottom_right.x) <= tolerance)
			{
				m_b_is_axis_aligned = true;
			}
		}

		if (m_b_is_axis_aligned)
		{
			const FSlateRect rounded_axis_aligned_rect = FSlateRect(in_top_left.x, in_top_left.y, in_bottom_right.x, in_bottom_right.y);//todo:add round

			const float left = std::min(rounded_axis_aligned_rect.left, rounded_axis_aligned_rect.right);
			const float right = std::max(rounded_axis_aligned_rect.left, rounded_axis_aligned_rect.right);
			const float top = std::min(rounded_axis_aligned_rect.top, rounded_axis_aligned_rect.bottom);
			const float bottom = std::max(rounded_axis_aligned_rect.top, rounded_axis_aligned_rect.bottom);

			top_left = glm::vec2(left, top);
			top_right = glm::vec2(right, top);
			bottom_left = glm::vec2(left, bottom);
			bottom_right = glm::vec2(right, bottom);
		}
		else
		{
			top_left = in_top_left;
			top_right = in_top_right;
			bottom_left = in_bottom_left;
			bottom_right = in_bottom_right;
		}
	}
}