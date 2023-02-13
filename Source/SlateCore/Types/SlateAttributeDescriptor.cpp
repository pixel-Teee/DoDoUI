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
		const FSlateAttributeDescriptor::FAttribute* result = nullptr;

		//interms of the memory offset to find attribute
		auto iter = std::find_if(m_attributes.begin(), m_attributes.end(), [attribute_offset](const FAttribute& other)
		{
			return other.m_offset == attribute_offset
			&& other.m_attribute_type == SlateAttributePrivate::ESlateAttributeType::Member;
		});

		//result = &(*iter);

		if (iter != m_attributes.end()) result = &(*iter);

		return result;
	}

	FSlateAttributeDescriptor::FAttribute* FSlateAttributeDescriptor::find_attribute(
		DoDoUtf8String attribute_name)
	{
		auto iter = std::find_if(m_attributes.begin(), m_attributes.end(), [attribute_name](const FAttribute& other) { return other.m_name == attribute_name; });
	
		//return &(*iter);
		if (iter == m_attributes.end()) return nullptr;
		return &(*iter);
	}

	FSlateAttributeDescriptor::FAttribute::FAttribute(DoDoUtf8String name, OffsetType offset,
		FInvalidateWidgetReasonAttribute reason)
			: m_name(name)
			, m_offset(offset)
			, m_invalidation_reason(std::move(reason))
	{
	}

	FSlateAttributeDescriptor::FContainerInitializer::FAttributeEntry::FAttributeEntry(
		FSlateAttributeDescriptor& descriptor, DoDoUtf8String container_name, int32_t atttribute_index)
			: m_descriptor(descriptor)
			, m_container_name(container_name)
			, m_attribute_index(atttribute_index)
	{
	}

	FSlateAttributeDescriptor::FInitializer::FInitializer(FSlateAttributeDescriptor& in_descriptor)
		: m_descriptor(in_descriptor)
	{
	}

	FSlateAttributeDescriptor::FInitializer::FInitializer(FSlateAttributeDescriptor& in_descriptor,
		const FSlateAttributeDescriptor& parent_descriptor)
			: m_descriptor(in_descriptor)
	{
		//in_descriptor.m_attributes = parent_descriptor.m_attributes;//todo:add containers
	}

	FSlateAttributeDescriptor::FInitializer::~FInitializer()
	{
	}

	FSlateAttributeDescriptor::FInitializer::FAttributeEntry::FAttributeEntry(FSlateAttributeDescriptor& descriptor,
		int32_t in_attribute_index)
			: m_descriptor(descriptor)
			, m_attribute_index(in_attribute_index)
	{
	}

	FSlateAttributeDescriptor::FInitializer::FAttributeEntry& FSlateAttributeDescriptor::FInitializer::FAttributeEntry::affect_visibility()
	{
		//todo:check descriptor's attributes is valid index
		//call descriptor's set affect visibility
		return *this;
	}

	FSlateAttributeDescriptor::FInitializer::FAttributeEntry FSlateAttributeDescriptor::FInitializer::add_member_attribute(DoDoUtf8String attribute_name, OffsetType offset, FInvalidateWidgetReasonAttribute&& reason_getter)
	{
		//call descriptor's private function
		return m_descriptor.add_member_attribute(attribute_name, offset, std::move(reason_getter));
	}

}
