#pragma once

#include "Core/Delegates/Delegates.h"
#include "Slate/Widgets/Input/Reply.h"
#include "SlateCore/Input/Events.h"
#include "SlateCore/Layout/Geometry.h"

namespace DoDo
{
	/*delegate type for handling mouse events*/
	using FPointerEventHandler = Delegate<FReply(const FGeometry&, const FPointerEvent&)>;
}
