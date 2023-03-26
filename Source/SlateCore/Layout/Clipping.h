#pragma once

#include "Core/Core.h"

namespace DoDo {
	enum class EWidgetClipping : uint8_t
	{
		/*this widget clips content bounds of this widget, it intersects those bounds with any previous clipping area*/
		ClipToBounds
	};

	/*
	* the clipping zone represents some arbitrary plane segment that can be used to clip the geometry in slate
	*/
	class FSlateClippingZone
	{
	public:
		glm::vec2 top_left;
		glm::vec2 top_right;
		glm::vec2 bottom_left;
		glm::vec2 bottom_right;

		explicit FSlateClippingZone(const FGeometry& bounding_geometry);

		void set_should_intersect_parent(bool b_value)
		{
			m_b_interset = b_value;
		}

		void set_always_clip(bool b_value)
		{
			m_b_always_clip = b_value;
		}

	private:
		void initialize_from_arbitrary_points(const glm::vec2& in_top_left, const glm::vec2& in_top_right, const glm::vec2& in_bottom_left, const glm::vec2& in_bottom_right);
	private:
		uint8_t m_b_is_axis_aligned : 1;
		/*should this clipping zone intersect the current one?*/
		uint8_t m_b_interset : 1;

		/*should this clipping zone always clip, even if another zone wants to ignore intersection?*/
		uint8_t m_b_always_clip : 1;
	};
}