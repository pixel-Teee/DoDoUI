#pragma once

#include "Core/Delegates/DelegateCombinations.h"

//#include "Core/Delegates/Delegates.h"//Delegate

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
		//using FGetter = Delegate<ObjectType()>;

		DECLARE_DELEGATE_RetVal(ObjectType, FGetter);

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
		* constructs by binding an arbitrary function that will be called to generate this attribute's value an demand
		* after binding, the attribute will no longer have a value that can be accessed directly, and instead the bound
		* function will always be called to generate the value
		* 
		* @param InUserObject shared pointer to the instance of the class that contains the member function you want to bind, the attribute will only retain a weak pointer to this class
		* @param InMethodPtr member function to bind, the function's structure (return value, arguments, etc) must match IBoundAttributeDelegate's definition
		*/
		template<class SourceType>
		TAttribute(std::shared_ptr<SourceType> in_user_object, typename FGetter::template TConstMethodPtr<SourceType> in_method_ptr)
			: m_value()
			, m_b_is_set(true)
			, m_getter(FGetter::CreateSP(in_user_object, in_method_ptr))//todo:may be need weak ptr support
		{
		
		}

		/*
		 * static : creates an attribute that's pre-bound to the specified 'getter' delegate
		 *
		 * @param InGetter delegate to bind
		 */
		static TAttribute Create(const FGetter& in_getter)
		{
			const bool b_explicit_constructor = true;
			return TAttribute(in_getter, b_explicit_constructor);
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
			if(m_getter.is_bound())
			{
				m_value = m_getter.execute();
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
			return m_getter.is_bound();
		}

		const FGetter& Get_Binding() const
		{
			return m_getter;
		}

	private:
		/*special explicit constructor for TAttribute::Create()*/
		TAttribute(const FGetter& in_getter, bool b_explicit_constructor)
			: m_value()
			, m_b_is_set(true)
			, m_getter(in_getter)
		{}

		//current value, mutable so that we can cache the value locally when using a bound getter(allows const ref return value)
		mutable ObjectType m_value;

		//true, when this attribute was explicitly set by a consumer, false when the attribute's value is set to the default
		bool m_b_is_set;

		//bound member function this attribute(may be null if no function bound)
		//when set, all attempts to read the attribute's value will instead call this delegate to generate the value
		FGetter m_getter;
	};
}