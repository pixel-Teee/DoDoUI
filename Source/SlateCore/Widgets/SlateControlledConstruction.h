#pragma once

//#include <type_traits>

#include "Core/Templates/Identity.h"

#include "SlateCore/Types/SlateAttributeDescriptor.h"

namespace DoDo
{
	//forward declare
	class FSlateControlledConstruction;

	//todo:need to implement Private_Register Attributes static function
#define SLATE_DECLARE_WIDGET(WidgetType, ParentType) \
	private:\
		using Super = ParentType;\
		using ThisClass = WidgetType;\
		using PrivateThisType = WidgetType;\
		static const FSlateWidgetClassData& Get_Private_Widget_Class() \
		{\
			static FSlateWidgetClassData Widget_Class_Data_Instance = FSlateWidgetClassData(TIdentity<ParentType>(), #WidgetType, &WidgetType::Private_Register_Attributes);\
			return Widget_Class_Data_Instance;\
		}\
		static void Private_Register_Attributes(FSlateAttributeInitializer&);\
	public:\
		static const FSlateWidgetClassData& Static_Widget_Class() { return Get_Private_Widget_Class(); }\
		virtual const FSlateWidgetClassData& Get_Widget_Class() const override { return Get_Private_Widget_Class();}\
	private:

#define SLATE_IMPLEMENT_WIDGET(WidgetType)\
	FSlateWidgetClassRegistration ClassRegistration__##WidgetType = WidgetType::Static_Widget_Class();
	
	class FSlateWidgetClassData
	{
	private:
		friend FSlateControlledConstruction;
		FSlateWidgetClassData(DoDoUtf8String In_Widget_Type_Name)
			: m_widget_type(In_Widget_Type_Name)
		{
			
		}
	public:
		//this call back is Private_Register_Attributes, we need to implement this function in every widget, if we need to register some attribute
		template<typename InWidgetParentType>
		FSlateWidgetClassData(TIdentity<InWidgetParentType>, DoDoUtf8String InWidgetTypeName, void(*attribute_Initializer)(FSlateAttributeInitializer&))
			: m_widget_type(InWidgetTypeName)
		{
			//initializer the parent class if it's not already done
			const FSlateWidgetClassData& parent_widget_class_data = InWidgetParentType::Static_Widget_Class();

			//initialize the attribute descriptor
			//and pass parent widget class data as parameter to initializer
			FSlateAttributeInitializer initializer = { m_descriptor, parent_widget_class_data.Get_Attribute_Descriptor() };
			(*attribute_Initializer)(initializer);
		}

		const FSlateAttributeDescriptor& Get_Attribute_Descriptor() const { return m_descriptor; }

		DoDoUtf8String Get_Widget_Type() const { return m_widget_type; }
	public:
		//describe the static information about slate widget class
		//FSlateAttributeDescriptor is default construct
		FSlateAttributeDescriptor m_descriptor;

		DoDoUtf8String m_widget_type;
	};

	struct FSlateWidgetClassRegistration
	{
		FSlateWidgetClassRegistration(const FSlateWidgetClassData&) {}
	};

	class FSlateControlledConstruction
	{
	public:
		FSlateControlledConstruction() = default;
		virtual ~FSlateControlledConstruction() = default;

	public:
		//rewrite this function
		static const FSlateWidgetClassData& Static_Widget_Class()
		{
			//only a name?
			//note:an a descriptor?
			static FSlateWidgetClassData Instance = FSlateWidgetClassData("FSlateControlledConstruction");
			return Instance;
		}

		virtual const FSlateWidgetClassData& Get_Widget_Class() const = 0;

	private:
		//UI objects cannot be copy-constructed
		FSlateControlledConstruction(const FSlateControlledConstruction& other) = delete;

		//todo:widgets should only ever be constructed via SNew or SAssignNew
		void* operator new(const size_t In_Size)
		{
			return std::malloc(In_Size);
		}

	public:
		void operator delete(void* mem)
		{
			std::free(mem);
		}
	};
}
