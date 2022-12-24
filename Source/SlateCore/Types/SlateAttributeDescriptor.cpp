#include <PreCompileHeader.h>

#include "SlateAttributeDescriptor.h"

#include "SlateAttribute.h"//ESlateAttributeType.Member depends on it

namespace DoDo
{

	FSlateAttributeDescriptor::FInitializer::FAttributeEntry FSlateAttributeDescriptor::add_member_attribute(
		DoDoUtf8String attribute_name, OffsetType offset, FInvalidateWidgetReasonAttribute reason_getter)
	{
		int32_t new_index = -1;
		//check whether is already exist?
		FAttribute const* found_attribute = find_attribute(attribute_name);
	
		//todo:implement check
		//just to add FAttribute
		m_attributes.emplace_back(attribute_name, offset, reason_getter);
		new_index = m_attributes.size() - 1;
	
		return FInitializer::FAttributeEntry(*this, new_index);
	}

	const FSlateAttributeDescriptor::FAttribute* FSlateAttributeDescriptor::find_member_attribute(
		OffsetType attribute_offset) const
	{
		const FSlateAttributeDescriptor::FAttribute* result;

		//interms of the memory offset to find attribute
		auto iter = std::find_if(m_attributes.begin(), m_attributes.end(), [attribute_offset](const FAttribute& other)
		{
			return other.m_offset == attribute_offset
			&& other.m_attribute_type == SlateAttributePrivate::ESlateAttributeType::Member;
		});

		result = &(*iter);

		return result;
	}

	FSlateAttributeDescriptor::FAttribute* FSlateAttributeDescriptor::find_attribute(
		DoDoUtf8String attribute_name)
	{
		auto iter = std::find_if(m_attributes.begin(), m_attributes.end(), [attribute_name](const FAttribute& other) { return other.m_name == attribute_name; });
	
		return &(*iter);
	}
	
	FSlateAttributeDescriptor::FInitializer::FAttributeEntry FSlateAttributeDescriptor::FInitializer::add_member_attribute(DoDoUtf8String attribute_name, OffsetType offset, FInvalidateWidgetReasonAttribute&& reason_getter)
	{
		//call descriptor's private function
		return m_descriptor.add_member_attribute(attribute_name, offset, std::move(reason_getter));
	}

}
