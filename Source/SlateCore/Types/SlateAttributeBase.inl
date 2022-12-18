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

	private:
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