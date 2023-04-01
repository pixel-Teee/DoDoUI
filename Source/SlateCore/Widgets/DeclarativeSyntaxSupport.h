#pragma once

#include "SlateCore/Layout/Visibility.h"//FSlateBaseNamedArgs depends on it

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
#define SNew(WidgetType, ...) \
	MakeTDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::Make_Required_Args(__VA_ARGS__)) <<= WidgetType::FArguments()

	//just implement SAssignNew
	//expose as is populated variable
	//... is parameter
#define SAssignNew(ExposeAs, WidgetType, ...)\
	MakeTDecl<WidgetType>(#WidgetType, __FILE__, __LINE__, RequiredArgs::Make_Required_Args(__VA_ARGS__)) . Expose(ExposeAs) <<= WidgetType::FArguments()

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
		TAttribute<AttrType> _##AttrName

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
		 */\
		template<typename... VarTypes> \
		WidgetArgsType& AttrName##_Static( typename TAttribute< AttrType >::FGetter::template FStaticDelegate<VarTypes...>::FFuncPtr InFunc, VarTypes... Vars) \
		{ \
			_##AttrName = TAttribute< AttrType >::Create(TAttribute< AttrType >::FGetter::CreateStatic(InFunc, Vars...)); \
			return static_cast<WidgetArgsType*>(this)->Me();\
		} \
		/*bind attribute with delegate to a shared pointer-based class method, slate mostly uses shared pointer so we use an overload for this type of binding*/ \
		template<class UserClass, typename... VarTypes> \
		WidgetArgsType& AttrName(std::shared_ptr<UserClass> InUserObjectRef, typename TAttribute< AttrType >::FGetter::template TConstMethodPtr< UserClass, VarTypes... > InFunc, VarTypes... Vars)\
		{ \
			_##AttrName = TAttribute< AttrType >::Create( TAttribute< AttrType >::FGetter::CreateSP(InUserObjectRef, InFunc, Vars...));\
			return static_cast<WidgetArgsType*>(this)->Me();\
		} \
		/*bind attribute with delegate to a shared pointer-based class method, slate mostly uses shared pointers so we use an overload for this type of binding*/ \
		template<class UserClass, typename... VarTypes> \
		WidgetArgsType& AttrName(UserClass* InUserObject, typename TAttribute< AttrType >::FGetter::template TConstMethodPtr< UserClass, VarTypes... > InFunc, VarTypes... Vars) \
		{ \
			_##AttrName = TAttribute< AttrType >::Create( TAttribute< AttrType >::FGetter::CreateSP( InUserObject, InFunc, Vars... )); \
			return static_cast<WidgetArgsType*>(this)->Me();\
		} 
#define SLATE_PRIVATE_ARGUMENT_VARIABLE(ArgType, ArgName) \
		ArgType _##ArgName

#define SLATE_PRIVATE_ARGUMENT_FUNCTION(ArgType, ArgName) \
		WidgetArgsType& ArgName(ArgType InArg) \
		{ \
			_##ArgName = InArg;\
			return static_cast<WidgetArgsType*>(this)->Me();\
		}

	/*
	 * use this macro to add a attribute to the declaration of your widget
	 * an attribute can be a value or a function
	 */
#define SLATE_ATTRIBUTE(AttrType, AttrName) \
		SLATE_PRIVATE_ATTRIBUTE_VARIABLE(AttrType, AttrName); \
		SLATE_PRIVATE_ATTRIBUTE_FUNCTION(AttrType, AttrName)

	/*
	 * use this macro to declare a slate argument
	 * arguments differ from attributes in that they can only be values
	 */
#define SLATE_ARGUMENT(ArgType, ArgName) \
		SLATE_PRIVATE_ARGUMENT_VARIABLE(ArgType, ArgName); \
		SLATE_PRIVATE_ARGUMENT_FUNCTION(ArgType, ArgName)

/*
 * use this macro to declare a slate argument
 * arguments differ from attributes in that they can only be values
 */
#define SLATE_STYLE_ARGUMENT(ArgType, ArgName) \
		const ArgType* _##ArgName; \
		WidgetArgsType& ArgName(const ArgType* in_arg) \
		{\
			_##ArgName = in_arg; \
			return static_cast<WidgetArgsType*>(this)->Me(); \
		}

//todo:implement SLATE_SLOT_ARGUMENT

/*
 * use this macro between SLATE_BEGIN_ARGS and SLATE_END_ARGS
 * in order to add support for slots with the construct pattern
 */
#define SLATE_SLOT_ARGUMENT(SlotType, SlotName) \
		std::vector<typename SlotType::FSlotArguments> _##SlotName; \
		WidgetArgsType& operator + (typename SlotType::FSlotArguments& slot_to_add) \
		{ \
			_##SlotName.push_back(std::move(slot_to_add)); \
			return static_cast<WidgetArgsType*>(this)->Me(); \
		} \
		WidgetArgsType& operator + (typename SlotType::FSlotArguments&& slot_to_add) \
		{ \
			_##SlotName.push_back(std::move(slot_to_add)); \
			return static_cast<WidgetArgsType*>(this)->Me(); \
		} \


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
	template<class DeclarationType>
	struct NamedSlotProperty
	{
		NamedSlotProperty(DeclarationType& in_owner_declaration, TAlwaysValidWidget& content_slot)
			: m_owner_declaration(in_owner_declaration)
			, m_slot_content(content_slot)
		{}

		DeclarationType& operator[](const std::shared_ptr<SWidget>& in_child)
		{
			m_slot_content.m_widget = in_child;
			return m_owner_declaration;//return m_owner_declaration
		}

		DeclarationType& m_owner_declaration;//this is FArguments

		TAlwaysValidWidget& m_slot_content;
	};

	/*
	 * use this macro to add support named slot properties such as content and header
	 * see NamedSlotProperty for more details
	 */
#define SLATE_NAMED_SLOT(DeclarationType, SlotName) \
		NamedSlotProperty<DeclarationType> SlotName() \
		{ \
			return NamedSlotProperty<DeclarationType>(*this, _##SlotName);\
		} \
		TAlwaysValidWidget _##SlotName;

#define SLATE_DEFAULT_SLOT(DeclarationType, SlotName) \
		SLATE_NAMED_SLOT(DeclarationType, SlotName) ; \
		DeclarationType& operator[](const std::shared_ptr<SWidget>& in_child)\
		{ \
			_##SlotName.m_widget = in_child;\
			return static_cast<WidgetArgsType*>(this)->Me();\
		}
	/*
	 * use this macro to add event handler support to the declarative syntax of your widget
	 * it is expected that the widget has a delegate called of type 'EventDelegateType' that is
	 * named 'EventName'
	 */
#define SLATE_EVENT(DelegateName, EventName)\
		WidgetArgsType& EventName(const DelegateName& InDelegate) \
		{ \
			_##EventName = InDelegate; \
			return static_cast<WidgetArgsType*>(this)->Me();\
		} \
		\
		WidgetArgsType& EventName(DelegateName&& InDelegate) \
		{ \
			_##EventName = InDelegate; \
			return static_cast<WidgetArgsType*>(this)->Me();\
		} \
		/* set event delegate to a global function */\
		/* note: we use a template here to avoid 'typename' issues when hosting attributes inside templated classes */\
		template<typename StaticFuncPtr, typename... VarTypes> \
		WidgetArgsType& EventName##_Static(StaticFuncPtr InFunc, VarTypes... Vars )	\
		{ \
			_##EventName = DelegateName::CreateStatic(InFunc, Vars...); \
			return static_cast<WidgetArgsType*>(this)->Me(); \
		} \
		/*set event delegate to a shared pointer-based class method, slate mostly uses shared pointers so we can use an overload for this type of binding*/ \
		template<class UserClass, typename... VarTypes> \
		WidgetArgsType& EventName(std::shared_ptr<UserClass> in_user_object_ref, typename DelegateName::template TMethodPtr<UserClass, VarTypes...> in_func, VarTypes... Vars) \
		{ \
			_##EventName = DelegateName::CreateSP(in_user_object_ref, in_func, Vars...);\
			return static_cast<WidgetArgsType*>(this)->Me(); \
		} \
		template<class UserClass, typename... VarTypes> \
		WidgetArgsType& EventName(std::shared_ptr<UserClass> in_user_object_ref, typename DelegateName::template TConstMethodPtr<UserClass, VarTypes...> in_func, VarTypes... Vars) \
		{ \
			_##EventName = DelegateName::CreateSP(in_user_object_ref, in_func, Vars...);\
			return static_cast<WidgetArgsType*>(this)->Me(); \
		} \
		DelegateName _##EventName;

	/*
	 * base class for named arguments, provides settings necessary for all widgets
	 */
	struct FSlateBaseNamedArgs
	{
		FSlateBaseNamedArgs() = default;

		SLATE_PRIVATE_ATTRIBUTE_VARIABLE(EVisibility, Visibility) = EVisibility::visible;
		SLATE_PRIVATE_ATTRIBUTE_VARIABLE(bool, IsEnabled) = true;
		SLATE_PRIVATE_ATTRIBUTE_VARIABLE(glm::vec2, RenderTransformPivot) = glm::vec2(0.0f, 0.0f);

		std::vector<std::shared_ptr<ISlateMetaData>> m_meta_data;
	};

	/*base class for named arguments, provides settings necessary for all widgets*/
	template<typename WidgetType>
	struct TSlateBaseNamedArgs : public FSlateBaseNamedArgs
	{
		typedef typename WidgetType::FArguments WidgetArgsType;

		SLATE_PRIVATE_ATTRIBUTE_FUNCTION(EVisibility, Visibility)

		/*used by the named argument pattern as a safe way to 'return *this' for call-chaining purposes*/
		WidgetArgsType& Me()
		{
			return *(static_cast<WidgetArgsType*>(this));//return to most derived FArguments
		}

		/*add meta data to this widget*/
		WidgetArgsType& AddMetaData(std::shared_ptr<ISlateMetaData> in_meta_data)
		{
			m_meta_data.push_back(in_meta_data);
			return Me();
		}

		/*add meta data to this widget - convenience method - 1 argument*/
		template<typename MetaDataType, typename Arg0Type>
		WidgetType& AddMetaData(Arg0Type in_arg0)
		{
			m_meta_data.push_back(std::make_shared<MetaDataType>(in_arg0));
			return Me();
		}

		//todo:implement two arguments
	};


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

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
		struct T3RequiredArgs
		{
			T3RequiredArgs(Arg0Type&& in_arg0, Arg1Type&& in_arg1, Arg2Type&& in_arg2)
				: m_arg0(in_arg0)
				, m_arg1(in_arg1)
				, m_arg2(in_arg2)
			{}

			template<class WidgetType>
			void CallConstruct(const std::shared_ptr<WidgetType>& on_widget, const typename WidgetType::FArguments& with_named_args) const
			{
				on_widget->Construct(with_named_args, std::forward<Arg0Type>(m_arg0), std::forward<Arg1Type>(m_arg1), std::forward<Arg2Type>(m_arg2));
			}

			Arg0Type&& m_arg0;
			Arg1Type&& m_arg1;
			Arg2Type&& m_arg2;
		};

		//todo:fix this
		//todo:if we discard inline declaration, we will get error
		inline T0RequiredArgs Make_Required_Args()
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

		template<typename Arg0Type, typename Arg1Type, typename Arg2Type>
		T3RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&> Make_Required_Args(Arg0Type&& in_arg0, Arg1Type&& in_arg1, Arg2Type&& in_arg2)
		{
			return T3RequiredArgs<Arg0Type&&, Arg1Type&&, Arg2Type&&>(std::forward<Arg0Type>(in_arg0), std::forward<Arg1Type>(in_arg1), std::forward<Arg2Type>(in_arg2));
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
			//m_widget->CacheVolatility();//cache volatility
			m_widget->m_b_Is_Declarative_Syntax_Construction_Completed = true;//constructed complete
			return m_widget;
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
