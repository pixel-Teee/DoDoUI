#pragma once

namespace DoDo
{
	/*
	 * slate widgets are constructed SNew and SAssignNew
	 *
	 * TSharedRef<SButton> MyButton = SNew(SButton)
	 * or
	 * TSharedPtr<SButton> MyButton;
	 * SAssignNew(MyButton, SButton);
	 *
	 * using SNew and SAssignNew ensures that widgets are populated
	 */

	//just implement SAssignNew
	//expose as is populated variable
	//... is parameter
#define SAssignNew(ExposeAs, WidgetType, ...)\
	MakeTDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::MakeRequiredArgs(__VA_ARGS__)) . Expose(ExposeAs) <<= WidgetType::FArguments()

	/*
	 * widget authors can use SLATE_BEGIN_ARGS and SLATE_END_ARGS to add support
	 * for widget construction via SNew and SAssignNew
	 *
	 * SLATE_BEGIN_ARGS(SMyWidget)
	 *		, _PreferredWidth(150.0f)
	 *		, _ForegroundColor(FLinearColor::White)
	 *		{}
	 *
	 *		SLATE_ATTRIBUTE(float, PreferredWidth)
	 *		SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)
	 * SLATE_END_ARGS()
	 */

#define SLATE_BEGIN_ARGS(WidgetType) \
	public:\
	struct FArguments : public TSlateBaseNamedArgs<WidgetType> \
	{ \
		typedef FArguments WidgetArgsType;\
		inline FArguments()

#define SLATE_END_ARGS() \
	};

	/*
	 * use this macro between SLATE_BEGIN_ARGS and SLATE_END_ARGS
	 * in order to add support for slots with construct pattern
	 */
#define SLATE_PRIVATE_ATTRIBUTE_VARIABLE(AttrType, AttrName) \
		TAttribute<AttrType> _##AttrName;

#define SLATE_PRIVATE_ATTRIBUTE_FUNCTION(AttrType, AttrName) \
		WidgetArgsType& AttrName(TAttribute<AttrType> InAttribute) \
		{ \
			_##AttrName = std::move(InAttribute);\
			return static_cast<WidgetArgsType*>(this)->Me();\
		} \
		\
		/*
		 * bind attribute with delegate to a global function
		 * note : we use a template here to avoid 'typename' issues when hosting attributes inside templated classes
		 */
		//todo:implement bind function


	/*
	 * use this macro to add a attribute to the declaration of your widget
	 * an attribute can be a value or a function
	 */
#define SLATE_ATTRIBUTE(AttrType, AttrName) \
		SLATE_PRIVATE_ATTRIBUTE_VARIABLE(AttrType, AttrName) \
		SLATE_PRIVATE_ATTRIBUTE_FUNCTION(AttrType, AttrName)


//todo:implement SLATE_SLOT_ARGUMENT


#define SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType) \
	{\
		using WidgetArgsType = SlotType::FSlotArguments;\
		using SlotParentType::FSlotArguments::FSlotArguments;

#define SLATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType) \
		public:\
		struct FSlotArguments : public SlotParentType::FSlotArguments \
		SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType)

#define SLATE_SLOT_BEGIN_ARGS_OneMixin(SlotType, SlotParentType, Mixin1) \
		public:\
		struct FSlotArguments : public SlotParentType::FSlotArguments, public Mixin1::FSlotArgumentsMixin \
		SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType)

#define SLATE_SLOT_BEGIN_ARGS_TwoMixins(SlotType, SlotParentType, Mixin1, Mixin2) \
		public:\
		struct FSlotArguments : public SlotParentType::FSlotArguments, public Mixin1::FSlotArgumentsMixin, public Mixin2::FSlotArgumentsMixin \
		SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType)

#define SLATE_SLOT_END_ARGS() \
	};

	/*a widget reference that is always a valid pointer, defaults to SNullWidget*/
	struct TAlwaysValidWidget
	{
		TAlwaysValidWidget()
			: m_widget() //null widget
		{
			
		}

		std::shared_ptr<SWidget> m_widget;
	};

	/*
	 * we want to be able to do:
	 * SNew(ContainerWidget)
	 * .SomeContentArea()
	 * [
	 *		//child widgets go here
	 * ]
	 *
	 * NamedSlotProperty is a helper that will be returned by SomeContentArea()
	 */


	namespace RequiredArgs
	{
		struct T0RequiredArgs
		{
			T0RequiredArgs()
			{
			}

			template<class WidgetType>
			void CallConstruct(const std::shared_ptr<WidgetType>& on_widget, const typename WidgetType::FArguments& with_named_args) const
			{
				//your widget must implement void Construct(const FArguments& InArgs)
				on_widget->Construct(with_named_args);
			}
		};

		template<typename Arg0Type>
		struct T1RequiredArgs
		{
			T1RequiredArgs(Arg0Type&& in_arg0)
				: m_arg0(in_arg0)
			{
			}

			template<class WidgetType>
			void CallConstruct(const std::shared_ptr<WidgetType>& on_widget, const typename WidgetType::FArguments& with_named_args) const
			{
				on_widget->Construct(with_named_args, std::forward<Arg0Type>(m_arg0));
			}

			Arg0Type& m_arg0;
		};

		template<typename Arg0Type, typename Arg1Type>
		struct T2RequiredArgs
		{
			T2RequiredArgs(Arg0Type&& in_arg0, Arg1Type&& in_arg1)
				: m_arg0(in_arg0)
				, m_arg1(in_arg1)
			{
			}

			template<class WidgetType>
			void CallConstruct(const std::shared_ptr<WidgetType>& on_widget, const typename WidgetType::FArguments& with_named_args) const
			{
				on_widget->Construct(with_named_args, std::forward<Arg0Type>(m_arg0), std::forward<Arg1Type>(m_arg1));
			}

			Arg0Type& m_arg0;
			Arg1Type& m_arg1;
		};

		T0RequiredArgs Make_Required_Args()
		{
			return T0RequiredArgs();
		}

		template<typename Arg0Type>
		T1RequiredArgs<Arg0Type&&> Make_Required_Args(Arg0Type&& in_arg0)
		{
			return T1RequiredArgs<Arg0Type&&>(std::forward<Arg0Type>(in_arg0));
		}

		template<typename Arg0Type, typename Arg1Type>
		T2RequiredArgs<Arg0Type&&, Arg1Type&&> Make_Required_Args(Arg0Type&& in_arg0, Arg1Type&& in_arg1)
		{
			return T2RequiredArgs<Arg0Type&&, Arg1Type&&>(std::forward<Arg0Type>(in_arg0), std::forward<Arg1Type>(in_arg1));
		}
	}

	//todo:implement SUserWidget
	/*normal widgets are allocated directly by the TSlateDecl*/
	template<typename WidgetType>
	struct TWidgetAllocator
	{
		//todo:implement to create a widget type
		static std::shared_ptr<WidgetType> PrivateAllocatedWidget()
		{
			return std::make_shared<WidgetType>();
		}
	};

	/*
	 * utility class used during widget instantiation
	 * performs widget allocation and construction(important)
	 * ensures that debug info is set correctly
	 * returns TSharedRef to widget
	 *
	 * @see SNew
	 * @see SAssignNew
	 */
	template<class WidgetType, typename RequiredArgsPayloadType>
	struct TSlateDecl
	{
		TSlateDecl(const char* in_type, const char* in_file, int32_t on_line, RequiredArgsPayloadType&& in_required_args)
			: m_widget(TWidgetAllocator<WidgetType>::PrivateAllocatedWidget())//this is for allocate a new swidget in the heap
			, m_required_args(in_required_args)
		{
			//todo:implement set debug info for SWidget
		}
		/*
		 * initialize OutVarToInit with the widget that is being constructed
		 * @see SAssignNew
		 */
		template<class ExposeAsWidgetType>
		TSlateDecl& Expose(std::shared_ptr<ExposeAsWidgetType>& out_var_to_init)
		{
			out_var_to_init = m_widget;//assign to parameter
			return *this;
		}

		/*
		 * complete widget construction from InArgs
		 *
		 * @param InArgs NamedArguments from which to construct the widget
		 * @return a reference to the widget that we constructed
		 */
		std::shared_ptr<WidgetType> operator<<=(const typename WidgetType::FArguments& in_args) const
		{
			//todo:implement SWidgetConstruct
			m_widget->SWidgetConstruct(in_args);//call SWidget Constructor
			m_required_args.CallConstruct(m_widget, in_args);//todo:implement CallConstruct
			m_widget->CacheVolatility();//cache volatility
			m_widget->m_b_Is_Declarative_Syntax_Construction_Completed = true;//constructed complete
		}

		const std::shared_ptr<WidgetType> m_widget;//owner ship?
		RequiredArgsPayloadType& m_required_args;//required args
	};

	//template<typename WidgetType, typename RequiredArgsPayloadType>
	//todo:implement TSlateDecl<WidgetType, RequiredArgsPayloadType>
	template<typename WidgetType, typename RequiredArgsPayloadType>
	TSlateDecl<WidgetType, RequiredArgsPayloadType> MakeTDecl(const char* in_type, const char* in_file, int32_t on_line, RequiredArgsPayloadType&& in_required_args)
	{
		return TSlateDecl<WidgetType, RequiredArgsPayloadType>(in_type, in_file, on_line, std::forward<RequiredArgsPayloadType>(in_required_args));
	}
}
