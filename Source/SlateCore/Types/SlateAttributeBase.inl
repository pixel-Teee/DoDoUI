#pragma once

namespace SlateAttributePrivate
{
	template<typename ContainerType, typename InObjectType, typename InInvalidationReasonPredicate, typename InComparePredicateType,
	ESlateAttributeType InAttributeType>
	struct TSlateAttributeBase : public FSlateAttributeImpl
	{
	public:
		template<typename Attribute_Member_Type>
		friend struct TSlateMemberAttributeRef;

		using ObjectType = InObjectType;
		using FInvalidationReasonPredicate = InInvalidationReasonPredicate;
		using FGetter = typename TAttribute<ObjectType>::FGetter;//there is core, have a delegate
		using FComparePredicate = InComparePredicateType;

	public:
		//return slate attribute cached value, if the slate attribute is bound, the value will be cached at the end of the every frame
		const ObjectType& Get() const
		{
			return m_Value;
		}

		/*
		 * unbind the slate attribute and set it's value, it may invalidate the widget if the value is different
		 * return true if the value is considered different and an invalidation occurred
		 */
		bool Set(ContainerType& widget, const ObjectType& new_value)
		{
			
		}

		/*
		 * unbind the slate attribute and set it's value, it may invalidate the widget if the value is different
		 * return true if the value is considered differentand an invalidation occurred
		 */
		bool Set(ContainerType& widget, ObjectType&& new_value)
		{
			
		}

		bool Assign(ContainerType& widget, TAttribute<ObjectType>&& other_attribute)
		{
			//other_attribute bind a function
			if (other_attribute.Is_Bound())
			{
				return Assign_Binding(widget, other_attribute.Get_Binding());
			}//other_attribute does not bind a function, but assign a value
			else if(other_attribute.Is_Set())
			{
				return Set(widget, other_attribute.Get());
			}
			else
			{
				return false;
			}
		}

	private:
		bool Assign_Binding(ContainerType& widget, const FGetter& getter)
		{
			const FDelegateHandle Previous_Getter_Handler = protected_find_getter_handle(widget, InAttributeType);

			//if(Previous_Getter_Handler != getter.Get_Handle())
			//{
			//	//todo:write a construct wrapper
			//	return true;
			//}

			return false;
		}

		bool Assign_Binding(ContainerType& widget, FGetter&& getter)
		{
			
		}

		//------this is a getter interface's implementation------
		template<typename SlateAttributeType>
		class FSlateAttributeGetterWrapper : public ISlateAttributeGetter
		{
		public:
			using ObjectType = typename SlateAttributeType::Object_Type;
			using FGetter = typename TAttribute<ObjectType>::FGetter;
			using FComparePredicate = typename SlateAttributeType::FComparePredicate;

			FSlateAttributeGetterWrapper() = delete;

			FSlateAttributeGetterWrapper(const FSlateAttributeGetterWrapper&) = delete;

			FSlateAttributeGetterWrapper& operator = (const FSlateAttributeGetterWrapper&) = delete;

			virtual ~FSlateAttributeGetterWrapper() = default;

		public:
			FSlateAttributeGetterWrapper(SlateAttributeType& In_Owning_Attribute, const FGetter& In_Getter_Delegate)
				: m_getter(In_Getter_Delegate)
				, m_attribute(&In_Owning_Attribute)
			{

			}

			FSlateAttributeGetterWrapper(SlateAttributeType& In_Owning_Attribute, FGetter& In_Getter_Delegate)
				: m_getter(std::move(In_Getter_Delegate))
				, m_attribute(&In_Owning_Attribute)
			{

			}

		private:
			//getter function to fetch the new value of the slate attribute
			FGetter m_getter;
			//the slate attribute of the swidget owning the value
			SlateAttributeType* m_attribute;
		};
		//------this is a getter interface's implementation------
	private:
		ObjectType m_Value;
	};
}