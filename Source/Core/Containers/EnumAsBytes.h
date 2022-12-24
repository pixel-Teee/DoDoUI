#pragma once

namespace DoDo
{
	/*
	 * template to store enumeration values as bytes in a type-safe way
	 */
	template<class TEnum>
	class TEnumAsByte
	{
	public:
		typedef TEnum EnumType;

		TEnumAsByte() = default;

		TEnumAsByte(const TEnumAsByte&) = default;

		TEnumAsByte& operator=(const TEnumAsByte&) = default;

		/*
		 * constructor, initialize to the enum value
		 *
		 * @param InValue value to construct with
		 */
		TEnumAsByte(TEnum In_Value)
			: m_value(static_cast<uint8_t>(In_Value))
		{}

	private:
		//holds the value as a byte
		uint8_t m_value;
	};
}