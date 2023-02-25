#pragma once

#include "RangeBound.h"

namespace DoDo {
	template<typename ElementType>
	class TRange
	{
	public:
		typedef TRangeBound<ElementType> BoundsType;

		typedef typename TCallTraits<ElementType>::ParamType ElementValueOrConstRef;

		TRange(){}

		/*
		* creates a range with a single element
		* 
		* the created range is of the form [A, A]
		* 
		* @param A the element in the range
		*/
		explicit TRange(ElementValueOrConstRef A)
			: m_lower_bound(BoundsType::Inclusive(A))
			, m_upper_bound(BoundsType::Inclusive(A))
		{}

		/*
		* create and initializes a new range with the given lower and upper bounds
		* 
		* the created range is of the form [A, B)
		* 
		* @param A the range's lower bound value (inclusive)
		* @param B the range's upper bound value (exclusive)
		*/
		explicit TRange(ElementValueOrConstRef A, ElementValueOrConstRef B)
			: m_lower_bound(BoundsType::Inclusive(A))
			, m_upper_bound(BoundsType::Exclusive(B))
		{}

		explicit TRange(const BoundsType& in_lower_bound, const BoundsType& in_upper_bound)
			: m_lower_bound(in_lower_bound)
			, m_upper_bound(in_upper_bound)
		{}

		/*
		* returns an empty range
		* 
		* @return empty range
		* @see all, atlest, atmost, exclusive, greaterthen, inclusive, lessthen
		*/
		static TRange Empty()
		{
			return TRange(BoundsType::Exclusive(ElementType()), BoundsType::Exclusive(ElementType()));
		}

		/*
		* check whether this range contains the specified element
		* 
		* @param element the element to check
		* @return true if the range contains the element, false otherwise
		*/
		bool Contains(ElementValueOrConstRef element) const
		{
			return ((BoundsType::MinLower(m_lower_bound, element) == m_lower_bound) &&
				(BoundsType::MaxUpper(m_upper_bound, element) == m_upper_bound));
		}

	private:
		/*holds the range's lower bound*/
		BoundsType m_lower_bound;

		/*holds the range's upper bound*/
		BoundsType m_upper_bound;
	};

	/*
* default range for built-in types (for FProperty support)
*/
#define DEFINE_RANGEBOUND_WRAPPER_STRUCT(Name, ElementType)\
	struct Name : TRange<ElementType> \
	{ \
	private: \
		typedef TRange<ElementType> Super; \
	\
	public: \
		Name()\
			: Super()\
		{}\
		\
		Name(const Super& Other)\
			: Super(Other)\
		{}\
		\
		Name(const ElementType InValue)\
			: Super(InValue)\
		{}\
		\
		static inline Name Empty() \
		{\
			return Super::Empty();\
		}\
	};

	DEFINE_RANGEBOUND_WRAPPER_STRUCT(FInt32Range, int32_t)
}