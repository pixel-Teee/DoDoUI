#pragma once

#include "Core/Math/TransformCalculus2D.h"
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

		/*2d transform support*/
		glm::vec2 transform_point(const glm::vec2& point) const
		{
			return DoDo::transform_point(get_translation(), DoDo::transform_point(m_scale, point));
		}

		/*2d transform support*/
		glm::vec2 transform_vector(const glm::vec2& vector) const
		{
			return DoDo::transform_vector(get_translation(), DoDo::transform_vector(m_scale, vector));
		}

		/*
		 * invert the transform/scale
		 */
		//note:construct a inverse FSlateLayoutTransform
		FSlateLayoutTransform inverse() const
		{
			return FSlateLayoutTransform(DoDo::inverse(m_scale), DoDo::inverse(glm::vec2(m_translation)) * DoDo::inverse(m_scale));
		}

		/*
		 * this works by transforming the origin through lhs then rhs
		 * in matrix from, looks like this:
		 * [sa  0   0]   [sb  0   0]
		 * [0   sa  0] * [0   sb  0]
		 * [tax tay 1] * [tbx tby 1]
		 */
		FSlateLayoutTransform concatenate(const FSlateLayoutTransform& rhs) const
		{
			return FSlateLayoutTransform(DoDo::concatenate(m_scale, rhs.m_scale), rhs.transform_point(get_translation()));
		}

	private:
		float m_scale;//scale
		glm::vec2 m_translation;
	};

	//from layout transform to render transform
	//adapter
	template<> template<> inline FTransform2D TransformCoverter<FTransform2D>::convert<FSlateLayoutTransform>(const FSlateLayoutTransform& transform)
	{
		return FTransform2D(transform.get_scale(), transform.get_translation());
	}
}
