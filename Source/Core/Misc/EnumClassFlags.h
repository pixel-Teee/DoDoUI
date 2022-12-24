#pragma once

#include <type_traits>

namespace DoDo {
	//defines all bitwise operators for enum classes so it can be (mostly) used as a regular flags enum
#define ENUM_CLASS_FLAGS(Enum) \
	inline constexpr Enum& operator |= (Enum& lhs, Enum rhs) { return lhs = (Enum)((std::underlying_type<Enum>::type)lhs | (std::underlying_type<Enum>::type)rhs); }\
	inline constexpr Enum operator | (Enum lhs, Enum rhs) {  return (Enum)((std::underlying_type<Enum>::type)lhs | (std::underlying_type<Enum>::type)rhs); } \
	inline constexpr Enum operator & (Enum lhs, Enum rhs) {  return (Enum)((std::underlying_type<Enum>::type)lhs & (std::underlying_type<Enum>::type)rhs); }

	template<typename Enum>
	constexpr bool Enum_Has_Any_Flags(Enum flags, Enum contains)
	{
		//return ((std::underlying_type<Enum>::type)flags & (std::underlying_type<Enum>::type)contains) != 0;

		return (((typename std::underlying_type<Enum>::type)(flags)) & ((typename std::underlying_type<Enum>::type)(contains))) != 0;
	}
}