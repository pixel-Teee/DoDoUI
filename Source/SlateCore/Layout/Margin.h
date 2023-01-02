#pragma once

#include "glm/vec2.hpp"//FMargin depends on it
#include "glm/vec4.hpp"//FMargin depends on it
#include "SlateCore/Types/SlateEnums.h"//GetTotalSpaceAlong depens on it

namespace DoDo
{
	/*
	 * describes the space around a widget
	 */
	struct FMargin
	{
		float left;

		float top;

		float right;

		float bottom;

	public:
		/*
		 * default constructor
		 *
		 * the default margin size is zero on all four sides
		 */
		FMargin()
			: left(0.0f)
			, top(0.0f)
			, right(0.0f)
			, bottom(0.0f)
		{}

		/* construct a margin with uniform space on all sides */
		FMargin(float uniform_margin)
			: left(uniform_margin)
			, top(uniform_margin)
			, right(uniform_margin)
			, bottom(uniform_margin)
		{}

		/* construct a margin where horizontal describes left and right spacing while vertical describes top and bottom spacing */
		FMargin(float horizontal, float vertical)
			: left(horizontal)
			, top(vertical)
			, right(horizontal)
			, bottom(vertical)
		{}

		/* construct a margin where horizontal describes left and right spacing while vertical describes top and bottom spacing */
		FMargin(const glm::vec2& in_vec)
			: left(in_vec.x)
			, top(in_vec.y)
			, right(in_vec.x)
			, bottom(in_vec.y)
		{
			
		}

		/* construct a margin where the spacing on each side is individually specified */
		FMargin(float in_left, float in_top, float in_right, float in_bottom)
			: left(in_left)
			, top(in_top)
			, right(in_right)
			, bottom(in_bottom)
		{
			
		}

		/* construct a margin where the margins are coming from a FVector4 */
		FMargin(const glm::vec4& in_vector)
			: left(in_vector.x)
			, top(in_vector.y)
			, right(in_vector.z)
			, bottom(in_vector.w)
		{
			
		}

	public:
		/*
		 * multiply the margin by a scalar
		 *
		 * @param scale how much to scale the margin
		 * @return an FMargin where each value is scaled by scale
		 */
		FMargin operator*(float scale) const
		{
			return FMargin(left * scale, top * scale, right * scale, bottom * scale);
		}

		/*
		 * gets the margin's total size
		 *
		 * @return cumulative margin size
		 */
		glm::vec2 get_desired_size() const
		{
			return glm::vec2(left + right, top + bottom);
		}

		/*
		 * gets the total horizontal or vertical margin
		 *
		 * @return cumulative horizontal margin
		 */
		template<EOrientation Orientation>
		float Get_Total_Space_Along() const
		{
			return 0.0f;
		}
	};

	template<>
	inline float FMargin::Get_Total_Space_Along<Orient_Horizontal>() const { return left + right; }

	template<>
	inline float FMargin::Get_Total_Space_Along<Orient_Vertical>() const { return top + bottom; }
}
