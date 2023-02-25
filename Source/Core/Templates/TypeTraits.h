#pragma once

#include "AndOrNot.h"
#include "IsPODType.h"
#include "IsArithmetic.h"

namespace DoDo {
	/*
	* call traits helpers
	*/
	template<typename T, bool TypeIsSmall>
	struct TCallTraitsParamTypeHelper
	{
		typedef const T& ParamType;
		typedef const T& ConstParamType;
	};

	template<typename T>
	struct TCallTraitsParamTypeHelper<T, true>
	{
		typedef const T ParamType;
		typedef const T ConstParamType;
	};

	template<typename T>
	struct TCallTraitsParamTypeHelper<T*, true>
	{
		typedef T* ParamType;
		typedef const T* ConstParamType;
	};

	template<typename T>
	struct TCallTraitsBase
	{
	private:
		enum { PassByValue = TOr<TAndValue<(sizeof(T) <= sizeof(void*)), TIsPODType<T>>, TIsArithmetic<T>>::Value };

	public:
		typedef T ValueType;
		typedef T& Reference;
		typedef const T& ConstReference;
		typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ParamType ParamType;//if don't know to pass value or pass reference, use this type
		typedef typename TCallTraitsParamTypeHelper<T, PassByValue>::ConstParamType ConstPointerType;
	};
	/*
	* TCallTraits
	*/
	template<typename T>
	struct TCallTraits : public TCallTraitsBase<T> {};

	//fix reference-to-reference problems
	template<typename T>
	struct TCallTraits<T&>
	{
		typedef T& ValueType;
		typedef T& Reference;
		typedef const T& ConstReference;
		typedef T& ParamType;
		typename T& ConstPointerType;
	};

	//todo:implement array and const array

	//implement container type


}