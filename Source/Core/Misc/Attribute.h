#pragma once

#include "Core/Delegates/Delegates.h"

namespace DoDo {
	/*
		attribute object
	*/
	template<typename ObjectType>
	class TAttribute
	{
	public:
		/*
			attribute getter delegate

			ObjectType GetValue() const

			return the attribute's value
		*/
		using FGetter = Delegate<ObjectType()>;

		//default constructor
		TAttribute() : m_value()
			, m_b_is_set(false)
			, m_getter()
		{

		}

		//constructor implicitly from an initial value
		//param In_Initial_Value the value for this attribute
		template<typename OtherType>
		TAttribute(const OtherType& In_Initial_Value)
			: m_value(static_cast<ObjectType>(In_Initial_Value))
			, m_b_is_set(true)
			, m_getter()
		{

		}

		//construct implicitly from moving an initial value
		//param In_Initial_Value
		TAttribute(ObjectType&& In_Initial_Value)
			: m_value(std::move(In_Initial_Value))
			, m_b_is_set(true)
			, m_getter()
		{

		}

	private:
		//current value, mutable so that we can cache the value locally when using a bound getter(allows const ref return value)
		mutable ObjectType m_value;

		//true, when this attribute was explicitly set by a consumer, false when the attribute's value is set to the default
		bool m_b_is_set;

		//bound member function this attribute(may be null if no function bound)
		//when set, all attempts to read the attribute's value will instead call this delegate to generate the value
		FGetter m_getter;
	};
}