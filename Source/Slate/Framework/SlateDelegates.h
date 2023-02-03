#pragma once

#include "Core/Delegates/Delegates.h"

#include "Slate/Widgets/Input/Reply.h"

namespace DoDo
{
	/*
	 * a delegate that is invoked when widgets want to notify a user that they have been clicked
	 * intended for use by buttons and other button-like widgets
	 */
	typedef Delegate_Event<FReply()> FOnClicked;
}