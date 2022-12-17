#pragma once

namespace DoDo {
	/*
		attribute object
	*/
	template<typename ObjectType>
	class TAttribute
	{
	public:

	private:
		//current value, mutable so that we can cache the value locally when using a bound getter(allows const ref return value)
		mutable ObjectType m_value;

		//true, when this attribute was explicitly set by a consumer, false when the attribute's value is set to the default
		bool m_b_is_set;


	};
}