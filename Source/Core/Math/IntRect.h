#pragma once

#include "IntPoint.h"

namespace DoDo {
	/*
	* structure for integer rectangles in 2-d space
	*/
	struct FIntRect
	{
		/*holds th first pixel line/row (like in Win32 RECT)*/
		FIntPoint m_min;

		/*holds the last pixel line/row (like in Win32 RECT)*/
		FIntPoint m_max;

	public:
		/*constructor*/
		FIntRect();

		/*
		* constructor
		* 
		* @param X0 minimum X coordinate
		* @param Y0 minimum y coordinate
		* @param x1 maximum x coordinate
		* @param Y1 maximum y coordinate
		*/
		FIntRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1);

		/*
		* calculates the area of this rectangle
		* 
		* @return the area of this reactangle
		*/
		int32_t Area() const;
	};
}