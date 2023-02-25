#pragma once

#include "Core/Templates/TypeTraits.h"

#include "Core/Containers/EnumAsBytes.h"

namespace DoDo {
	namespace ERangeBoundTypes
	{
		/*
		* enumerates the valid types of range bounds
		*/
		enum Type
		{
			/*the range excludes the bound*/
			Exclusive,

			/*the range includes the bound*/
			Inclusive,

			/*the bound is open*/
			Open
		};
	}
	/*
	* template for range bounds
	*/
	template<typename ElementType>
	class TRangeBound
	{
	public:
		/*
		* typedef used to pass/return small element types by value rather than const&
		*/
		typedef typename TCallTraits<ElementType>::ParamType ElementValueOrConstRef;//note:determine to pass value or pass reference

		/*
		* defaluts constructor
		* 
		* @see Exclusive, Inclusive, Open
		*/
		TRangeBound()
			: Type(ERangeBoundTypes::Open)
			, Value()
		{}

		/*
		* creates a closed bound that includes the specified value
		* 
		* @param InValue the bound's value
		* @see Exclusive, Inclusive, Open
		*/
		TRangeBound(ElementValueOrConstRef InValue)
			: Type(ERangeBoundTypes::Inclusive)
			, Value(InValue)
		{}

		/*
		* compares this bound with the specified bound for equality
		* 
		* @param Other the bound to compare with
		* @param true if the bounds are equal, false otherwise
		*/
		bool operator==(const TRangeBound& other) const
		{
			return ((Type == other.Type) && (IsOpen() || (Value == other.Value)));
		}

		/*
		* returns a closed bound that includes the specified value
		* 
		* @param value the bound value
		* @return an inclusive closed bound
		*/
		static TRangeBound Inclusive(ElementValueOrConstRef Value)
		{
			TRangeBound result;

			result.Type = ERangeBoundTypes::Inclusive;
			result.Value = Value;

			return result;
		}

		/*
		* returns a closed bound that includes the specified value
		* 
		* @param Value the bound value
		* @return an exclusive closed bound
		*/
		static TRangeBound Exclusive(ElementValueOrConstRef Value)
		{
			TRangeBound Result;

			Result.Type = ERangeBoundTypes::Exclusive;
			Result.Value = Value;

			return Result;
		}

		bool IsInclusive() const
		{
			return (Type == ERangeBoundTypes::Inclusive);
		}

		bool IsOpen() const
		{
			return (Type == ERangeBoundTypes::Open);
		}

		static const TRangeBound& MinLower(const TRangeBound& A, const TRangeBound& B)
		{
			if (A.IsOpen()) { return A; }
			if (B.IsOpen()) { return B; }
			if (A.Value < B.Value) { return A; }
			if (B.Value < A.Value) { return B; }
			if (A.IsInclusive()) { return A; }

			return B;
		}

		static const TRangeBound& MaxUpper(const TRangeBound& A, const TRangeBound& B)
		{
			if (A.IsOpen()) { return A; }
			if (B.IsOpen()) { return B; }
			if (A.Value > B.Value) { return A; }
			if (B.Value > A.Value) { return B; }
			if (A.IsInclusive()) { return A; }

			return B;
		}

	private:
		/*holds the type of the bound*/
		TEnumAsByte<ERangeBoundTypes::Type> Type;

		/*holds the bound's value*/
		ElementType Value;
	};

}