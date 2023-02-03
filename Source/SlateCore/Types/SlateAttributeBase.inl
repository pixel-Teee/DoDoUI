#pragma once

namespace SlateAttributePrivate
{
	//ContainerType is SWidget
	//InObjectType is color or scale etc
	//InComparePredicateType is TSlateAttributeComparePredicate
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

		static EInvalidateWidgetReason Get_Invalidation_Reason(const SWidget& widget) { return FInvalidationReasonPredicate::Get_InvalidationReason(widget); }
		static EInvalidateWidgetReason Get_Invalidation_Reason(const ISlateAttributeContainer& container) { return FInvalidationReasonPredicate::Get_InvalidationReason(container.Get_Container_Widget()); }
		//use == to compare lhs and rhs
		static bool Identical_To(const SWidget& widget, const ObjectType& lhs, const ObjectType& rhs) { return FComparePredicate::Identical_To(widget, lhs, rhs); }
		static bool Identical_To(const ISlateAttributeContainer& container, const ObjectType& lhs, const ObjectType& rhs) { return FComparePredicate::Identical_To(container.Get_Container_Widget(), lhs, rhs); }
	public:
		TSlateAttributeBase()
			: m_Value()
		{

		}

		TSlateAttributeBase(const ObjectType& In_Value)
			: m_Value(In_Value)
		{
			
		}

		TSlateAttributeBase(ObjectType&& In_Value)
			: m_Value(std::move(In_Value))
		{
			
		}

		TSlateAttributeBase(SWidget& widget)
			: m_Value()
		{
			
		}

		TSlateAttributeBase(SWidget& widget, const ObjectType& In_Value)
			: m_Value(In_Value)
		{

		}

		TSlateAttributeBase(SWidget& widget, ObjectType&& In_Value)
			: m_Value(std::move(In_Value))
		{

		}

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
			const bool b_Is_Identical = Identical_To(widget, m_Value, new_value);

			//unregister attribute and mark different flag on widget
			protected_unregister_attribute(widget, InAttributeType);

			if(!b_Is_Identical)
			{
				m_Value = new_value;
				protected_invalidate_widget(widget, InAttributeType, Get_Invalidation_Reason(widget));
			}

			return !b_Is_Identical;
		}

		/*
		 * unbind the slate attribute and set it's value, it may invalidate the widget if the value is different
		 * return true if the value is considered different an invalidation occurred
		 */
		bool Set(ContainerType& widget, ObjectType&& new_value)
		{
			const bool b_Is_Identical = Identical_To(widget, m_Value, new_value);//check equal
			//todo:implement ProtectedUnregisterAttribute
			//may be register some getter, need to unbind
			protected_unregister_attribute(widget, InAttributeType);
			if(!b_Is_Identical)
			{
				m_Value = std::move(m_Value);
				//todo:implement Protected Invalidate Widget
			}

			return !b_Is_Identical;
		}

		//from TAttribute to construct TSlateAttributeBase
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
			//from widget meta data(FSlateAttributeMetaData) to query whether this slate attribute is registered?
			//compare getter
			const FDelegateHandle Previous_Getter_Handler = protected_find_getter_handle(widget, InAttributeType);

			//if(Previous_Getter_Handler != getter.Get_Handle())
			//{
			//	//todo:write a construct wrapper
			//	return true;
			//}

			if (Previous_Getter_Handler != getter.Get_Handle())
			{
				Construct_Wrapper(widget, std::move(getter));

				return true;
			}

			return false;
		}

		bool Assign_Binding(ContainerType& widget, FGetter&& getter)
		{
			
		}

		void Construct_Wrapper(ContainerType& widget, const FGetter& getter)
		{
			//make a FSlateAttributeGetterWrapper<TSlateAttributeBase>
			std::unique_ptr<ISlateAttributeGetter> wrapper = Make_Unique_Getter(*this, getter);
			
			protected_register_attribute(widget, InAttributeType, std::move(wrapper));

			//update now on bind
		}

		void Construct_Wrapper(ContainerType& widget, FGetter&& getter)
		{
			std::unique_ptr<ISlateAttributeGetter> wrapper = Make_Unique_Getter(*this, std::move(getter));

			protected_register_attribute(widget, InAttributeType, std::move(wrapper));

			//update now on bind
		}

		//------this is a getter interface's implementation------
		template<typename SlateAttributeType>
		class FSlateAttributeGetterWrapper : public ISlateAttributeGetter
		{
		public:
			using ObjectType = typename SlateAttributeType::ObjectType;
			using FGetter = typename TAttribute<ObjectType>::FGetter;
			using FComparePredicate = typename SlateAttributeType::FComparePredicate;

			FSlateAttributeGetterWrapper() = delete;

			FSlateAttributeGetterWrapper(const FSlateAttributeGetterWrapper&) = delete;

			FSlateAttributeGetterWrapper& operator = (const FSlateAttributeGetterWrapper&) = delete;

			virtual ~FSlateAttributeGetterWrapper() override = default;

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

			virtual FDelegateHandle Get_Delegate_Handle() const override
			{
				return m_getter.Get_Handle();
			}

			FUpdateAttributeResult Update_Attribute(const SWidget& widget) override
			{
				return FUpdateAttributeResult();
			}

			const FSlateAttributeBase& Get_Attribute() const override
			{
				return *m_attribute;
			}

			void Set_Attribute(FSlateAttributeBase&) override
			{
				//
			}

		private:		
			//getter function to fetch the new value of the slate attribute
			FGetter m_getter;
			//the slate attribute of the swidget owning the value
			SlateAttributeType* m_attribute;
		};
		//------this is a getter interface's implementation------
	private:
		static std::unique_ptr<ISlateAttributeGetter> Make_Unique_Getter(TSlateAttributeBase& attribute, const FGetter& getter)
		{
			return std::make_unique<FSlateAttributeGetterWrapper<TSlateAttributeBase>>(attribute, getter);
		}

		static std::unique_ptr<ISlateAttributeGetter> Make_Unique_Getter(TSlateAttributeBase& attribute, FGetter&& getter)
		{
			return std::make_unique<FSlateAttributeGetterWrapper<TSlateAttributeBase>>(attribute, std::move(getter));
		}

		ObjectType m_Value;
	};
}