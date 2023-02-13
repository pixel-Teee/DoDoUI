#pragma once

#include "glm/vec2.hpp"

namespace DoDo
{
	enum EForceInit
	{
		ForceInit,
		ForceInitToZero
	};

	/*
	 * implements a rectangular 2d box
	 */
	template<typename T>
	struct TBox2
	{
	public:
		using FReal = T;//template parameter

		/*holds the box's minimum point*/
		glm::vec<2, T> m_min;

		/*holds the box's maximum point*/
		glm::vec<2, T> m_max;

		/*holds a flag indicating whether this box is valid*/
		bool m_b_is_valid;

	public:

		/*default constructor (no initialization)*/
		TBox2<T>() {}

		/*
		 * creates and initializes a new box
		 *
		 * the box extents are initialized to zero and the box is marked as invalid
		 *
		 * @param EForceInit Force Init Enum
		 */
		explicit TBox2<T>(EForceInit)
		{
			//call init
		}

		/*
		* gets the box size
		* 
		* @return box size
		* @see GetArea, GetCenter, GetCenterAndExtents, GetExtent
		*/
		glm::vec<2, T> get_size() const
		{
			return glm::vec<2, T>(m_max - m_min);
		}

		/*
		 * set the initial values of the bounding box to zero
		 */
		void Init()
		{
			m_min = m_max = glm::vec<2, T, glm::defaultp>(0.0f, 0.0f);
			m_b_is_valid = false;
		}
	};

	//typedef
	typedef TBox2<float> FBox2f;
	typedef TBox2<double> FBox2D;
}
