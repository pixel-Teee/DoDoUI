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

		/*
		 * set the attribute's value
		 *
		 * param InNewValue the value to set the attribute to
		 */
		template<typename OtherType>
		void Set(const OtherType& InNewValue)
		{
			//unbind delegate
			//todo:need to unbind the delegate
			m_value = InNewValue;
			m_b_is_set = true;
		}

		/*
		 * set the attribute's value
		 *
		 * param InNewValue the value to set the attribute to
		 */
		void Set(ObjectType&& InNewValue)
		{
			//todo:need to unbind the delegate
			m_value = std::move(InNewValue);
			m_b_is_set = true;
		}

		//was this TAttribute ever assigned?
		bool Is_Set() const
		{
			return m_b_is_set;
		}

		/*
		 * gets the attribute's current value
		 * assumes that the attribute is set
		 *
		 * return the attribute's value
		 */
		const ObjectType& Get() const
		{
			//if we have a getter delegate, then we'll call that to generate the value
			if(m_getter.Is_Bound())
			{
				m_value = m_getter.Execute();
			}

			//return stored value
			return m_value;
		}

		/*
		 * gets the attribute's current value, the attribute may not be set, in which case use the default value provided
		 * shorthand for the boilerplate code: MyAttribute.IsSet() ? MyAttribute.Get() : DefaultValue
		 */
		const ObjectType& Get(const ObjectType& default_value) const
		{
			return m_b_is_set ? Get() : default_value;
		}

		/*
		 * checks to see if this attribute has a 'getter' function bound
		 * return true if attribute is bound to a getter function
		 */
		bool Is_Bound() const
		{
			return m_getter.Is_Bound();
		}

		const FGetter& Get_Binding() const
		{
			return m_getter;
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