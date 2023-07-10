#pragma once

#include "Core/Delegates/DelegateCombinations.h"

#include "SlateCore/Input/Reply.h"

#include "SlateCore/Types/SlateEnums.h"//ETextCommit depends on it

namespace DoDo
{
	class DoDoUtf8String;
	class ITableRow;
	//DECLARE_DELEGATE_RetVal(
	//	FReply,
	//	FOnClicked
	//)
	/*
	 * a delegate that is invoked when widgets want to notify a user that they have been clicked
	 * intended for use by buttons and other button-like widgets
	 */
	DECLARE_DELEGATE_RetVal(FReply, FOnClicked)

	/*notification for float value changed*/
	DECLARE_DELEGATE_OneParam(FOnFloatValueChanged, float)

	/*notification for FLinearColor value changed*/
	DECLARE_DELEGATE_OneParam(FOnLinearColorValueChanged, FLinearColor)

	/*
	* delegate for hooking up to an editable text box's 'OnTextCommitted'
	* 
	* this first parameter (NewText) is the new text string
	* 
	* the second parameter contains information about how the text was committed
	*/
	DECLARE_DELEGATE_TwoParams(FOnTextCommitted, const DoDoUtf8String&, ETextCommit::Type)

	template<typename ArgumentType>
	class TSlateDelegates
	{
		/*
		* the advanced version of OnGenerateWidget
		* you are given the reference to the owning list/tree and asked to return the appropriate container widget
		*/

		DECLARE_DELEGATE_RetVal_TwoParams(
			/* return: the widget visualization of the item*/
			std::shared_ptr<ITableRow>,
			FOnGenerateRow,
			/*param: an item to visualize*/
			ArgumentType,
			/*param: the owning widget*/
			const std::shared_ptr<class STableViewBase>&
		);
	};
}