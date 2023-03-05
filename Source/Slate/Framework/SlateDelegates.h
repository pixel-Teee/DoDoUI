#pragma once

#include "Core/Delegates/DelegateCombinations.h"

#include "Slate/Widgets/Input/Reply.h"

namespace DoDo
{
	/*
	 * a delegate that is invoked when widgets want to notify a user that they have been clicked
	 * intended for use by buttons and other button-like widgets
	 */
	DECLARE_DELEGATE_RetVal(FReply, FOnClicked)

	/*notification for float value changed*/
	DECLARE_DELEGATE_OneParam(FOnFloatValueChanged, float)
}