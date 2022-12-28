#pragma once

#include <glm/glm.hpp>

namespace DoDo
{
	/*represents a 2d non-uniform scale(to disambiguate from an FVector2D, which is used for translation)*/
	class FScale2D
	{
	public:
		/* ctor, initialize to an identity scale, 1.0 */
		FScale2D() : m_scale(1.0f, 1.0f){}

		/* ctor, initialize from a uniform scale */
		explicit FScale2D(float in_scale) : m_scale(in_scale, in_scale) {}


		/* access to the underlying FVector2D that stores the scale */
		const glm::vec2& get_vector() const { return m_scale; }
	private:
		/*underlying storage of the 2D scale*/
		glm::vec2 m_scale;
	};

	/*
	 * 2x2 generalized matrix
	 * as FMatrix, we assume row vectors, row major storage
	 * [X Y] * [m00 m01]
	 *		   [m10 m11]
	 */
	class FMatrix2x2
	{
	public:
		/*ctor initialize to an identity*/
		FMatrix2x2()
		{
			m_m[0][0] = 1; m_m[0][1] = 0;
			m_m[1][0] = 0; m_m[1][1] = 1;
		}

		explicit FMatrix2x2(const FScale2D& scale)
		{
			float scale_x = scale.get_vector().x;
			float scale_y = scale.get_vector().y;

			m_m[0][0] = scale_x; m_m[0][1] = 0;
			m_m[1][0] = 0; m_m[1][1] = scale_y;
		}

	private:
		float m_m[2][2];
	};

	/*
	 * support for generalized 2d affine transforms
	 * implemented as a 2x2 transform followed by translation. in matrix form:
	 * [A B 0]
	 * [C D 0]
	 * [X Y 1]
	 */
	class FTransform2D
	{
	public:
		/* initialize the transform using an identity matrix and a translation */
		FTransform2D(const glm::vec2& translation = glm::vec2(0.0f, 0.0f))
			: m_trans(translation)
		{
			
		}

		/* initialize the transform using a uniform scale an a translation */
		explicit FTransform2D(float uniform_scale, const glm::vec2& translation = glm::vec2(0.0f, 0.0f))
			: m_m(FScale2D(uniform_scale)), m_trans(translation)
		{
			//FScale2D scale_2d(uniform_scale);

			
		}


	private:
		FMatrix2x2 m_m;//matrix
		glm::vec2 m_trans;
	};
}