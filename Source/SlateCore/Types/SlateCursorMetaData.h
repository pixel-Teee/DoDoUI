#pragma once

#include "SlateCore/Types/ISlateMetaData.h"

#include "Core/Misc/Attribute.h"//TAttribute depends on it

#include <optional>


#include "ApplicationCore/GenericPlatform/ICursor.h"//EMouseCursor::Type depends on it

namespace DoDo {
	/*meta data that holds the cursor content for a widget*/
	class FSlateCursorMetaData : public ISlateMetaData
	{
	public:
		SLATE_METADATA_TYPE(FSlateCursorMetaData, ISlateMetaData)

		/*the cursor to show when the mouse is hovering over this widget*/
		TAttribute<std::optional<EMouseCursor::Type>> m_cursor;
	};
}