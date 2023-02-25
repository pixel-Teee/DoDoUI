#pragma once

#include "Core/Core.h"

namespace DoDo {
	/*
	* traits class which tests if a type is arithmetic
	*/
	template<typename T>
	struct TIsArithmetic
	{
		enum {Value = false};
	};

	template <> struct TIsArithmetic<float> { enum { Value = true }; };
	template <> struct TIsArithmetic<double> { enum { Value = true }; };
	template <> struct TIsArithmetic<long double> { enum { Value = true }; };
	template <> struct TIsArithmetic<uint8_t> { enum { Value = true }; };
	template <> struct TIsArithmetic<uint16_t> { enum { Value = true }; };
	template <> struct TIsArithmetic<uint32_t> { enum { Value = true }; };
	template <> struct TIsArithmetic<uint64_t> { enum { Value = true }; };
	template <> struct TIsArithmetic<int8_t>   { enum { Value = true }; };
	template <> struct TIsArithmetic<int16_t>  { enum { Value = true }; };
	template <> struct TIsArithmetic<int32_t>  { enum { Value = true }; };
	template <> struct TIsArithmetic<int64_t>  { enum { Value = true }; };
	template <> struct TIsArithmetic<long>     { enum { Value = true }; };
	template <> struct TIsArithmetic<unsigned long> { enum { Value = true }; };
	template <> struct TIsArithmetic<bool> { enum { Value = true }; };
	template <> struct TIsArithmetic<char> { enum { Value = true }; };

	template <typename T> struct TIsArithmetic<const T> { enum { Value = TIsArithmetic<T>::Value }; };
	template <typename T> struct TIsArithmetic<volatile T> { enum { Value = TIsArithmetic<T>::Value }; };
	template <typename T> struct TIsArithmetic<const volatile T> { enum { Value = TIsArithmetic<T>::Value }; };
}