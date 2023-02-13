#include <PreCompileHeader.h>

#include "SlotBase.h"

#include "Layout/ChildrenBase.h"
#include "Widgets/SWidget.h"

namespace DoDo
{
	FSlotBase::FSlotBase()
		: m_owner(nullptr)
		, m_widget()
	{
	}

	FSlotBase::FSlotBase(const FChildren& in_parent)
		: m_owner(&in_parent)//note:important
		, m_widget()
	{
	}

	FSlotBase::~FSlotBase()
	{
	}

	SWidget* FSlotBase::get_owner_widget() const
	{
		//first to call FSlotBase's function, get_owner, to get the FChildren that own the FSlotBase
		//second to call FChildren's function, get_owner, to get the SWidget
		if(get_owner())
		{
			return &(get_owner()->get_owner());
		}
		else
		{
			return nullptr;
		}
	}

	void FSlotBase::set_owner(const FChildren& children)
	{
		if(m_owner != &children)
		{
			m_owner = &children;

			after_content_or_owner_assigned();
		}
	}

	void FSlotBase::detach_parent_from_content()
	{
		//todo:implement SNullWidget::NullWidget
		if(m_widget != nullptr)
		{
			m_widget->conditionally_detach_parent_widget(get_owner_widget());
		}
	}

	void FSlotBase::after_content_or_owner_assigned()
	{
		if(SWidget* owner_widget = get_owner_widget())
		{
			if(m_widget != nullptr)
			{
				//todo:implement assign parent widget
				m_widget->assign_parent_widget(owner_widget->shared_from_this());
			}
		}
	}
}
