#pragma once

#include "Core/Core.h"

namespace DoDo {
	/*
	* types of easing functions for slate animation curves, these are used to smooth out animations
	*/
	enum class ECurveEaseFunction : uint8_t
	{
		/*linear interpolation, with no easing*/
		Linear,

		/*Quadratic ease in*/
		QuadIn,

		/*Quadratic ease out*/
		QuadOut,

		/*Quadratic ease in, quadratic ease out*/
		QuadInOut,

		/*cubic ease in*/
		CubicIn,

		/*cubic ease out*/
		CubicOut,

		/*cubic ease in, cubic ease out*/
		CubicInOut
	};
}