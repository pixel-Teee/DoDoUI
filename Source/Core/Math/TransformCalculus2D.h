#pragma once

#include <glm/glm.hpp>

namespace DoDo
{
	/*
	 * inverts a transform from space a to space b so it transforms from space b to space a
	 * by default attempts to call a member function on the transform type
	 *
	 * @param Transform Input transform from space a to space b
	 * @return inverted transform from space b to space a
	 */
	template<typename TransformType>
	inline auto inverse(const TransformType& transform) -> decltype(transform.inverse())
	{
		return transform.inverse();//call member function
	}
	/*
	 * generic implementation of transform point for 2d vectors with double precision, attempts to use a member function of the transform type
	 */
	template<typename TransformType>
	inline glm::vec2 transform_point(const TransformType& transform, const glm::vec2& point)
	{
		return transform.transform_point(point);//note: call TransformType's member function, to transform point
	}

	/*specialization for FVector2D translation*/
	//note:this is for glm::vec2
	inline glm::vec2 transform_point(const glm::vec2& transform, const glm::vec2& point)
	{
		return transform + point;
	}

	inline glm::vec2 transform_point(float transform, const glm::vec2& point)
	{
		return transform * point;
	}

	/*
	 * generic implementation of TransformVector for 2d vectors, attempts to use a member function of the TransformType
	 */
	template<typename TransformType>
	inline glm::vec2 transform_vector(const TransformType& transform, const glm::vec2& vector)
	{
		return transform.transform_vector(vector);//note:call member function
	}

	/*
	 * specialization for FVector2d translation(does noting)
	 */
	//note:does nothing
	inline const glm::vec2& transform_vector(const glm::vec2& transform, const glm::vec2& vector)
	{
		return vector;
	}

	/*
	 * specialization for uniform scale
	 */
	inline glm::vec2 transform_vector(float transform, const glm::vec2& vector)
	{
		return transform * vector;
	}

	/*
	 * inverts a transform from space a to space b so it transforms from space b to space a
	 * by default attempts to call a member function on the transform type
	 *
	 * @param Transform Input transform from space a to space b
	 * @return inverted transform from space b to space a
	 */
	inline float inverse(float scale)
	{
		return 1.0f / scale;
	}

	/*specialization for inverting a 2d translation*/
	//template<typename T>
	inline glm::vec2 inverse(const glm::vec2& transform)//todo:need add inline, for one definition rule
	{
		return -transform;
	}

	template<class TransformType>
	struct TransformCoverter
	{
		static const TransformType& convert(const TransformType& transform)
		{
			return transform;
		}

		template<typename OtherTransformType>
		static TransformType convert(const OtherTransformType& transform)
		{
			return TransformType(transform);//from one type to construct another type object
		}
	};

	//from one type to construct another type object
	template<typename ResultType, typename TransformType>
	auto TransformCast(const TransformType& transform) -> decltype(TransformCoverter<ResultType>::convert(transform))
	{
		return TransformCoverter<ResultType>::convert(transform);
	}

	template<typename TransformTypeA, typename TransformTypeB>
	struct ConcatenateRules
	{
		
	};

	/*
	 * concatenates two transforms, use transformCast<> to convert them first
	 * if more efficient means are available to concatenate two transforms, provide a non-template overload(or possibly a specialization)
	 * concatenation is performed in left to right order, so the output space of lhs must match the input space of rhs
	 *
	 * @param lhs transformation that goes from space a to space b
	 * @param rhs transformation that goes from space b to space c
	 * @return a new transform representing the transformation from the input space of lhs to the output space of rhs
	 */
	template<typename TransformTypeA, typename TransformTypeB>
	inline typename ConcatenateRules<TransformTypeA, TransformTypeB>::ResultType concatenate(const TransformTypeA& lhs, const TransformTypeB& rhs)
	{
		typedef typename ConcatenateRules<TransformTypeA, TransformTypeB>::ResultType ReturnType;

		return concatenate(TransformCast<ReturnType>(lhs), TransformCast<ReturnType>(rhs));
	}

	template<typename TransformType>
	inline auto concatenate(const TransformType& lhs, const TransformType& rhs) -> decltype(lhs.concatenate(rhs))
	{
		return lhs.concatenate(rhs);
	}

	inline float concatenate(float lhs, float rhs)
	{
		return lhs * rhs;
	}

	template<typename TransformType1, typename TransformType2, typename TransformType3>
	inline auto concatenate(const TransformType1& transform_a_to_b, const TransformType2& transform_b_to_c, const TransformType3& transform_c_to_d)
		-> decltype(concatenate(concatenate(transform_a_to_b, transform_b_to_c), transform_c_to_d))
	{
		return concatenate(concatenate(transform_a_to_b, transform_b_to_c), transform_c_to_d);
	}

	template<typename TransformType1, typename TransformType2, typename TransformType3, typename TransformType4>
	inline auto concatenate(const TransformType1& transform_a_to_b, const TransformType2& transform_b_to_c, const TransformType3& transform_c_to_d, const TransformType4& transform_d_to_e)
		-> decltype(concatenate(concatenate(transform_a_to_b, transform_b_to_c, transform_c_to_d), transform_d_to_e))
	{
		return concatenate(concatenate(transform_a_to_b, transform_b_to_c, transform_c_to_d), transform_d_to_e);
	}

	template<typename TransformType1, typename TransformType2, typename TransformType3, typename TransformType4, typename TransformType5>
	inline auto concatenate(const TransformType1& transform_a_to_b, const TransformType2& transform_b_to_c, const TransformType3& transform_c_to_d, const TransformType4& transform_d_to_e, const TransformType5& transform_e_to_f)
		-> decltype(concatenate(concatenate(transform_a_to_b, transform_b_to_c, transform_c_to_d, transform_d_to_e), transform_e_to_f))
	{
		return concatenate(concatenate(transform_a_to_b, transform_b_to_c, transform_c_to_d, transform_d_to_e), transform_e_to_f);
	}

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

		FMatrix2x2(float m00, float m01, float m10, float m11)
		{
			m_m[0][0] = m00, m_m[0][1] = m01;
			m_m[1][0] = m10, m_m[1][1] = m11;
		}

		void get_matrix(float& a, float& b, float& c, float& d) const
		{
			a = m_m[0][0];
			b = m_m[0][1];
			c = m_m[1][0];
			d = m_m[1][1];
		}

		/*
		 * transform a 2d point
		 * [X Y] * [m00 m01]
		 *		   [m10 m11]
		 */
		glm::vec2 transform_point(const glm::vec2& point) const
		{
			return glm::vec2(point.x * m_m[0][0] + point.y * m_m[1][0],
				point.y * m_m[0][1] + point.y * m_m[1][1]);
		}

		/*
		 * concatenate 2 matrices:
		 * [A B] * [E F] == [AE+BG AF+BH]
		 * [C D] * [G H] == [CE+DG CF+DH]
		 */
		FMatrix2x2 concatenate(const FMatrix2x2& rhs) const
		{
			float a, b, c, d;
			get_matrix(a, b, c, d);
			float e, f, g, h;
			rhs.get_matrix(e, f, g, h);
			return FMatrix2x2(
				a * e + b * g, a * f + b * h,
				c * e + d * g, c * f + d * h
			);
		};

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

		explicit FTransform2D(const FMatrix2x2& transform, const glm::vec2& translation = glm::vec2(0.0f, 0.0f))
			: m_m(transform), m_trans(translation)
		{
			
		}

		const FMatrix2x2& get_matrix() const { return m_m; }

		/*access to the translation*/
		const glm::vec2 get_translation() const { return m_trans; }

		void set_translation(const glm::vec2& in_trans) { m_trans = in_trans; }

		/*
		 * concatenates two transforms, result is equivalent to transforming first by this, followed by rhs
		 * concat(A, B) == (P * MA + TA) * MB + TB
		 *				== (P * MA * MB) + TA * MB + TB
		 *
		 *	NewM == MA * MB
		 *	NewT == TA * MB + TB
		 */
		FTransform2D concatenate(const FTransform2D& rhs) const
		{
			//DoDo::transform_point(rhs.m_m, m_trans);
			return FTransform2D(
				DoDo::concatenate(m_m, rhs.m_m),//todo:implement
				DoDo::transform_point(rhs.m_m, m_trans) + rhs.m_trans)//todo:implement
			;
		}

		/*
		 * 2d transformation of a point, transforms position, rotation, and scale
		 */
		glm::vec2 transform_point(const glm::vec2& point) const
		{
			return DoDo::transform_point(get_translation(), DoDo::transform_point(m_m, point));
		}

	private:
		FMatrix2x2 m_m;//matrix
		glm::vec2 m_trans;
	};

	//inline FTransform2D concatenate(const glm::vec2)
	inline glm::vec2 concatenate(const glm::vec2& lhs, const glm::vec2& rhs)
	{
		return lhs + rhs;
	}

	/*specialization for concatenating 2d translation and transform, see docs for FTransform2D::inverse for details on how this math is derived*/
	inline FTransform2D concatenate(const glm::vec2& translation, const FTransform2D& transform)
	{
		//DoDo::concatenate(DoDo::transform_point(transform.get_matrix(), translation), transform.get_translation());
		return FTransform2D(transform.get_matrix(), concatenate(DoDo::transform_point(transform.get_matrix(), translation), transform.get_translation()));
	}

	//if two types don't equal, then return FTransform2D
	template<typename TransformType> struct ConcatenateRules<TransformType, typename std::enable_if<!std::is_same<FTransform2D, TransformType>::value, FTransform2D>::type> { typedef FTransform2D ResultType; };
	template<typename TransformType> struct ConcatenateRules<typename std::enable_if<!std::is_same<FTransform2D, TransformType>::value, FTransform2D>::type, TransformType> { typedef FTransform2D ResultType; };
	template<> struct ConcatenateRules <FMatrix2x2, FTransform2D> { typedef FTransform2D ResultType; };
	template<> struct ConcatenateRules <FTransform2D, FMatrix2x2> { typedef FTransform2D ResultType; };
}