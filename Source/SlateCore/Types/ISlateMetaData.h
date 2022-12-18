#pragma once

#include "Core/String/DoDoString.h"

namespace DoDo {
	/*
		base class for all slate metadata
	*/
	class ISlateMetaData
	{
	public:
		//check if this meta data operation can cast safely to the specified type
		template<class TType>
		bool Is_Of_Type() const
		{
			return Is_Of_Type_Impl(TType::Get_Type_Id());
		}

		//virtual destructor
		virtual ~ISlateMetaData() {}

	protected:
		/*
			checks whether this drag and drop operation can cast safely to the specified type
		*/
		virtual bool Is_Of_Type_Impl(const DoDoUtf8String& type) const
		{
			return false;
		}
	};

	/*
		all meta data derived classes must include this macro
		example usage:
			class FMyMetaData : public ISlateMetaData
			{
			public:
				SLATE_METADATA_TYPE(FMyMetaData, ISlateMetaData)
			};
	*/
#define SLATE_METADATA_TYPE(TYPE, BASE)\
	static const DoDoUtf8String& Get_Type_Id() { static DoDoUtf8String Type(#TYPE); return Type; } \
	virtual bool Is_Of_Type_Impl(const DoDoUtf8String& type) const override { return Get_Type_Id() == type || BASE::Is_Of_Type_Impl(type); }

	/*
		simple tagging meta data
	*/

	class FTagMetaData : public ISlateMetaData
	{
	public:
		SLATE_METADATA_TYPE(FTagMetaData, ISlateMetaData)
		
		FTagMetaData(DoDoUtf8String In_Tag)
			: m_tag(In_Tag)
		{}

		/*
			tag name for a widget
		*/
		DoDoUtf8String m_tag;
	};
}