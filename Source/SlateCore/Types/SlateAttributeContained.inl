#pragma once



namespace SlateAttributePrivate
{
	template<typename InObjectValue, typename InInvalidationReasonPredicate, typename InComparePredicate>
	struct TSlateContainedAttribute : public TSlateAttributeBase<ISlateAttributeContainer, InObjectValue, InInvalidationReasonPredicate, InComparePredicate, ESlateAttributeType::Contained>
	{
	private:
		using Super = TSlateAttributeBase<ISlateAttributeContainer, InObjectValue, InInvalidationReasonPredicate, InComparePredicate, ESlateAttributeType::Contained>;

		//note:ContainerType must derived from ISlateAttributeContainer
		template<typename ContainerType, typename U = typename std::enable_if<std::is_base_of<ISlateAttributeContainer, ContainerType>::value>::type>
		static void verify_attribute_address(const ContainerType& container, const TSlateContainedAttribute& self)
		{
			//todo:check address
		}

	public:
		using FGetter = typename Super::FGetter;//note:this is a real delegate
		using ObjectType = typename Super::ObjectType;

	public:
		TSlateContainedAttribute() = delete;
		TSlateContainedAttribute(const TSlateContainedAttribute&) = delete;
		TSlateContainedAttribute(TSlateContainedAttribute&&) = delete;
		TSlateContainedAttribute& operator=(const TSlateContainedAttribute&) = delete;
		TSlateContainedAttribute& operator=(TSlateContainedAttribute&&) = delete;//because construct is deleted, you need to construct attribute from object type value

		//todo:implement some check function and construction function
		//note:this is important, first parameter is container type, difference between slate member attribute
		template<typename ContainerType, typename U = typename std::enable_if<std::is_base_of<ISlateAttributeContainer, ContainerType>::value>::type>
		explicit TSlateContainedAttribute(ContainerType& container)
		{
			//todo:check address
		}

		template<typename ContainerType, typename U = typename std::enable_if<std::is_base_of<ISlateAttributeContainer, ContainerType>::value>::type>
		explicit TSlateContainedAttribute(ContainerType& container, const ObjectType& in_value)
			: Super(in_value)
		{
			//todo:check address
		}

		template<typename ContainerType, typename U = typename std::enable_if<std::is_base_of<ISlateAttributeContainer, ContainerType>::value>::type>
		explicit TSlateContainedAttribute(ContainerType& container, ObjectType&& in_value)
			: Super(std::move(in_value))
		{
			//todo:check address
		}
	};
}
