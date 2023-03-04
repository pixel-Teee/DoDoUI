#pragma once

#include "Core/String/DoDoString.h"

#include "Core/Templates/TypeHash.h"

#include <cmath>

namespace DoDo
{
	/*
	 * structure for integer points in 2-d space
	 */
	struct FIntPoint
	{
		union
		{
			struct
			{
				/*holds the point's x-coordinate*/
				int32_t x;

				/*holds the point's y-coordinate*/
				int32_t y;
			};

			//internal use only
			int32_t XY[2];
		};

	public:
		/*an integer point with zeroed values*/
		static const FIntPoint Zero_Value;

		/*an integer point with INDEX_NONE values*/
		static const FIntPoint None_Value;

	public:
		/*default constructor (no initialization)*/
		FIntPoint();

		/*
		 * create and initialize a new instance with the specified coordinates
		 *
		 * @param InX the x-coordinate
		 * @param InY the y-coordinate
		 */
		FIntPoint(int32_t In_X, int32_t In_Y);

		/*
		 * create and initialize a new instance with a single int
		 * both x and y will be initialized to this value
		 *
		 * @param InXY the x and y-coordinate
		 */
		FIntPoint(int32_t In_XY);

	public:
		/*
		 * get specific component of a point
		 *
		 * @param PointIndex Index of point component
		 * @return const reference to component
		 */
		const int32_t& operator()(int32_t point_index) const;
		
		/*
		 * get specific component of a point
		 *
		 * @param PointIndex Index of point component
		 * @return reference to component
		 */
		int32_t& operator()(int32_t point_index);

		/*
		 * compare two points for equality
		 *
		 * @param other the other int point begin compared
		 * @return true if the points are equal, false otherwise
		 */
		bool operator==(const FIntPoint& other) const;

		/*
		 * compare two points for inequality
		 *
		 * @param other the other int point being compared
		 * @return true if the points are not equal, false otherwise
		 */
		bool operator!=(const FIntPoint& other) const;

		/*
		 * scale this point
		 *
		 * @param scale what to multiply the point by
		 * @return reference to this point after multiplication
		 */
		FIntPoint& operator*=(int32_t scale);

		/*
		 * divide this point by a scalar
		 *
		 * @param divisor what to divide the point by
		 * @return reference to this point after division
		 */
		FIntPoint& operator/=(int32_t divisor);

		/*
		 * add other point component-wise to this point
		 *
		 * @param other the point to add to this point
		 * @return reference to this point after addition
		 */
		FIntPoint& operator+=(const FIntPoint& other);

		/*
		 * multiply another point component-wise from this point
		 *
		 * @param other the point to multiply with this point
		 * @return reference to this point after multiplication
		 */
		FIntPoint& operator*=(const FIntPoint& other);

		/*
		 * subtract another point component-wise from this point
		 *
		 * @param other the point to subtract from this point
		 * @return reference to this point after subtraction
		 */
		FIntPoint& operator-=(const FIntPoint& other);

		/*
		 * divide this point component-wise by another point
		 *
		 * @param other the point to divide with
		 * @return reference to this point after division
		 */
		FIntPoint& operator/=(const FIntPoint& other);

		/*
		 * assign another point to this one
		 *
		 * @param other the point to assign this point from
		 * @return reference to this point after assignment
		 */
		FIntPoint& operator=(const FIntPoint& other);

		/*
		 * get the result of scaling on this point
		 *
		 * @param scale what to multiply the point by
		 * @return a new scaled int point
		 */
		FIntPoint operator*(int32_t scale) const;

		/*
		 * get the result of division on this point
		 *
		 * @param divisor what to divide the point by
		 * @return A new divided int point
		 */
		FIntPoint operator/(int32_t divisor) const;

		/*
		 * get the result of addition on this point
		 *
		 * @param other the other point to add to this
		 * @return a new combined int point
		 */
		FIntPoint operator+(const FIntPoint& other) const;

		/*
		 * get the result of subtraction from this point
		 *
		 * @param other the other point to subtract from this
		 * @return a new subtracted int point
		 */
		FIntPoint operator-(const FIntPoint& other) const;

		/*
		 * get the result of multiplication on this point
		 *
		 * @param other the point to multiply with this point
		 * @return a new multiplied int point
		 */
		FIntPoint operator*(const FIntPoint& other) const;

		/*
		 * get the result of division on this point
		 *
		 * @param other the other point to subtract from this
		 * @return a new subtracted int point
		 */
		FIntPoint operator/(const FIntPoint& other) const;

		/*
		 * get specific component of the point
		 *
		 * @param index the index of point component
		 * @return reference to component
		 */
		int32_t& operator[](int32_t index);

		/*
		 * get specific component of the point
		 *
		 * @param index the index of point component
		 * @return copy of component value
		 */
		int32_t operator[](int32_t index) const;

	public:
		/*
		 * get the component-wise min of two points
		 *
		 * @see ComponentMax, GetMax
		 */
		FIntPoint Component_Min(const FIntPoint& other) const;

		/*
		 * get the component-wise max of two points
		 *
		 * @see ComponentMin, GetMin
		 */
		FIntPoint Component_Max(const FIntPoint& other) const;

		/*
		 * get the larger of the point's two components
		 *
		 * @return the maximum component of the point
		 * @see GetMin, Size, SizeSquared
		 */
		int32_t Get_Max() const;

		/*
		 * get the smaller of the point's two components
		 *
		 * @return the minimum component of the point
		 * @see GetMax, Size, SizeSquared
		 */
		int32_t Get_Min() const;

		/*
		 * get the distance of this point from (0, 0)
		 *
		 * @return the distance of this point from (0, 0)
		 * @see GetMax, GetMin, SizeSquared
		 */
		int32_t Size() const;

		/*
		 * get the squared distance of this point from (0, 0)
		 *
		 * @return the squared distance of this point from (0, 0)
		 * @see GetMax, GetMin, Size
		 */
		int32_t Size_Squared() const;

		/*
		 * get a textual representation of this point
		 *
		 * @return a string describing the point
		 */
		DoDoUtf8String To_String() const;

	public:
		/*
		 * divide an int point and round up the result
		 *
		 * @param lhs the int point begin divided
		 * @param divisor what to divide the int point by
		 * @return a new divided int point
		 * @see DivideAndRoundDown
		 */
		static FIntPoint Divide_And_Round_Up(FIntPoint lhs, int32_t divisor);

		static FIntPoint Divide_And_Round_Up(FIntPoint lhs, FIntPoint divisor);

		/*
		 * divide an int point and round down the result
		 *
		 * @param lhs the int point being divided
		 * @param divisor what to divide the int point by
		 * @return a new divided int point
		 * @see DivideAndRoundUp
		 */
		static FIntPoint Divide_And_Round_Down(FIntPoint lhs, int32_t divisor);

		/*
		 * get number of components point has
		 *
		 * @return number of components point has
		 */
		static int32_t Num();

	public:
		
		 //todo:implement serialize function


	};

	/*------FIntPoint inline functions------*/
	inline FIntPoint::FIntPoint()
	{
	}

	inline FIntPoint::FIntPoint(int32_t In_X, int32_t In_Y)
		: x(In_X)
		, y(In_Y)
	{
	}

	inline FIntPoint::FIntPoint(int32_t In_XY)
		: x(In_XY)
		, y(In_XY)
	{
	}

	inline const int32_t& FIntPoint::operator()(int32_t point_index) const
	{
		return XY[point_index];
	}

	inline int32_t& FIntPoint::operator()(int32_t point_index)
	{
		return XY[point_index];
	}

	inline bool FIntPoint::operator==(const FIntPoint& other) const
	{
		return x == other.x && y == other.y;
	}

	inline bool FIntPoint::operator!=(const FIntPoint& other) const
	{
		return (x != other.x) || (y != other.y);
	}

	inline FIntPoint& FIntPoint::operator*=(int32_t scale)
	{
		x *= scale;
		y *= scale;

		return *this;
	}

	inline FIntPoint& FIntPoint::operator/=(int32_t divisor)
	{
		x /= divisor;
		y /= divisor;

		return *this;
	}

	inline FIntPoint& FIntPoint::operator+=(const FIntPoint& other)
	{
		x += other.x;
		y += other.y;

		return *this;
	}

	inline FIntPoint& FIntPoint::operator*=(const FIntPoint& other)
	{
		x *= other.x;
		y *= other.y;

		return *this;
	}

	inline FIntPoint& FIntPoint::operator-=(const FIntPoint& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	inline FIntPoint& FIntPoint::operator/=(const FIntPoint& other)
	{
		x /= other.x;
		y /= other.y;

		return *this;
	}

	inline FIntPoint& FIntPoint::operator=(const FIntPoint& other)
	{
		x = other.x;
		y = other.y;

		return *this;
	}

	inline FIntPoint FIntPoint::operator*(int32_t scale) const
	{
		return FIntPoint(*this) *= scale;
	}

	inline FIntPoint FIntPoint::operator/(int32_t divisor) const
	{
		return FIntPoint(*this) /= divisor;
	}

	inline FIntPoint FIntPoint::operator+(const FIntPoint& other) const
	{
		return FIntPoint(*this) += other;
	}

	inline FIntPoint FIntPoint::operator-(const FIntPoint& other) const
	{
		return FIntPoint(*this) -= other;
	}

	inline FIntPoint FIntPoint::operator*(const FIntPoint& other) const
	{
		return FIntPoint(*this) *= other;
	}

	inline FIntPoint FIntPoint::operator/(const FIntPoint& other) const
	{
		return FIntPoint(*this) /= other;
	}

	inline int32_t& FIntPoint::operator[](int32_t index)
	{
		//todo:check index
		return ((index == 0) ? x : y);
	}

	inline int32_t FIntPoint::operator[](int32_t index) const
	{
		//todo:check index
		return ((index == 0) ? x : y);
	}

	inline FIntPoint FIntPoint::Component_Min(const FIntPoint& other) const
	{
		return FIntPoint(std::min(x, other.x), std::min(y, other.y));
	}

	inline FIntPoint FIntPoint::Component_Max(const FIntPoint& other) const
	{
		return FIntPoint(std::max(x, other.x), std::max(y, other.y));
	}

	inline int32_t FIntPoint::Get_Max() const
	{
		return std::max(x, y);
	}

	inline int32_t FIntPoint::Get_Min() const
	{
		return std::min(x, y);
	}

	inline int32_t FIntPoint::Size() const
	{
		int64_t x64 = (int64_t)x;
		int64_t y64 = (int64_t)y;

		return int32_t(std::sqrt(float(x64 * x64 + y64 * y64)));
	}

	inline int32_t FIntPoint::Size_Squared() const
	{
		return x * x + y * y;
	}

	inline DoDoUtf8String FIntPoint::To_String() const
	{
		//todo:need to implement printf to string
		return DoDoUtf8String();
	}

	inline FIntPoint FIntPoint::Divide_And_Round_Up(FIntPoint lhs, int32_t divisor)
	{
		//todo:need to implement math's divide and round up
		return FIntPoint();
	}

	inline FIntPoint FIntPoint::Divide_And_Round_Up(FIntPoint lhs, FIntPoint divisor)
	{
		return FIntPoint();
	}

	inline FIntPoint FIntPoint::Divide_And_Round_Down(FIntPoint lhs, int32_t divisor)
	{
		return FIntPoint();
	}

	inline int32_t FIntPoint::Num()
	{
		return 2;
	}

	inline uint32_t Get_Type_Hash(const FIntPoint& in_point)
	{
		return Hash_Combine(Get_Type_Hash(in_point.x), Get_Type_Hash(in_point.y));
	}
	/*------FIntPoint inline functions------*/
}
