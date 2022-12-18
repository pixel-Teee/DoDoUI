#pragma once

#include <Core/Core.h>

namespace DoDo
{
	class ISlateMetaData;

	class SWidget
	{
	public:
		SWidget();

		virtual ~SWidget();

		//return true if the widgets has any bound slate attribute
		bool Has_Registered_Slate_Attribute() const { return m_b_has_registered_slate_attribute; }

		//todo:move this member to private control
		//meta data associated with this widget
		std::vector<Scope<ISlateMetaData>> m_Meta_Data;
	private:

		//is there at least one slate attribute currently registered
		uint8_t m_b_has_registered_slate_attribute : 1;
	};
}