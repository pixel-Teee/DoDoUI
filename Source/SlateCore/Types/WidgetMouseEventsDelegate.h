#pragma once

#include "Core/Delegates/Delegate.h"
#include "Slate/Widgets/Input/Reply.h"
#include "SlateCore/Input/Events.h"
#include "SlateCore/Layout/Geometry.h"

namespace DoDo
{
	/*delegate type for handling mouse events*/
	//using FPointerEventHandler = Delegate<FReply(const FGeometry&, const FPointerEvent&)>;

	DECLARE_DELEGATE_RetVal_TwoParams(FReply, FPointerEventHandler, const FGeometry&, const FPointerEvent&);
}
