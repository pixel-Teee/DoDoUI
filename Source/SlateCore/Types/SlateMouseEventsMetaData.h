#pragma once

#include "ISlateMetaData.h"

#include "WidgetMouseEventsDelegate.h"

namespace DoDo
{
	/*metadata to hold mouse event set on the SWidget*/
	class FSlateMouseEventsMetaData : public ISlateMetaData
	{
	public:
		SLATE_METADATA_TYPE(FSlateMouseEventsMetaData, ISlateMetaData)

		FPointerEventHandler m_mouse_move_handler;
	};
}
