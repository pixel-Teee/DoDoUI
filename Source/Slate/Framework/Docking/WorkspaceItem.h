#pragma once

#include "Core/Core.h"

#include "SlateCore/Textures/SlateIcon.h"

#include "Core/String/DoDoString.h"

#include "Core/Misc/Attribute.h"//TAttribute depends on it

namespace DoDo {
	class FWorkspaceItem : public std::enable_shared_from_this<FWorkspaceItem>
	{
	protected:
		FWorkspaceItem(const DoDoUtf8String& in_display_name, const FSlateIcon& in_icon, const bool b_in_sort_children)
		: m_icon(in_icon)
		, m_display_name_attribute(in_display_name)
		, m_b_sort_children(b_in_sort_children)
		{}

		const FSlateIcon& get_icon() const
		{
			return m_icon;
		}

		const DoDoUtf8String& get_display_name() const
		{
			return m_display_name_attribute.Get();
		}

		FSlateIcon m_icon;

		TAttribute<DoDoUtf8String> m_display_name_attribute;
		
		bool m_b_sort_children;
	};
}