#pragma once

namespace DoDo {
	/*
	* does a boolean and of the ::value static members of each type, but short-circuits if any Type::Value == false
	*/
	template<typename... Types>
	struct TAnd;

	template<bool LHSValue, typename... RHS>
	struct TAndValue
	{
		static constexpr bool Value = TAnd<RHS...>::value;
		static constexpr bool value = TAnd<RHS...>::value;
	};

	template<typename... RHS>
	struct TAndValue<false, RHS...>//note:will stop at this class
	{
		static constexpr bool Value = false;
		static constexpr bool value = false;
	};

	template<typename LHS, typename... RHS>
	struct TAnd<LHS, RHS...> : TAndValue<LHS::Value, RHS...>
	{

	};

	template<>
	struct TAnd<>
	{
		static constexpr bool Value = true;
		static constexpr bool value = true;
	};

	/*
	* does a boolean or of the ::value static member of each type, but short-circuits if any type::value == true
	*/
	template<typename... Types>
	struct TOr;

	template<bool LHSValue, typename... RHS>
	struct TOrValue
	{
		static constexpr bool Value = TOr<RHS...>::value;
		static constexpr bool value = TOr<RHS...>::value;
	};

	template<typename... RHS>
	struct TOrValue<true, RHS...> //note:will stop in this class
	{
		static constexpr bool Value = true;
		static constexpr bool value = true;
	};

	template<typename LHS, typename... RHS>
	struct TOr<LHS, RHS...> : TOrValue<LHS::Value, RHS...>
	{

	};

	template<>
	struct TOr<>
	{
		static constexpr bool Value = false;
		static constexpr bool value = false;
	};
}