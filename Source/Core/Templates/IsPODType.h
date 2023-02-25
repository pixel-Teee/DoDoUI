#pragma once

#include <type_traits>

namespace DoDo {
	/*
	* traits class which tests if a type is POD
	*/
	template<typename T>
	struct TIsPODType
	{
		//enum { Value = std::is_trivial<T>::value; };
		static constexpr bool Value = std::is_trivial<T>::value;
	};
}