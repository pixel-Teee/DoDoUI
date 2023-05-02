#pragma once

#include "Core/Core.h"

#include "ReplyBase.h"//TReplyBase

namespace DoDo {

	enum class EPopupMethod : uint8_t
	{
		/*creating a new window allows us to place popups outside of the window in which the menu anchor resides*/
		CreateNewWindow,
		/*place the popup into the current window, applications that intend to run in fullscreen cannot create new windows, so they must use this method...*/
		UseCurrentWindow
	};

	enum class EShouldThrottle : uint8_t
	{
		No,
		Yes
	};

	/*reply informs slate how it should express the popup:by creating a new window or by reusing the existing window*/
	class FPopupMethodReply : public TReplyBase<FPopupMethodReply>
	{
	public:
		/*create a reply that signals not having an opinion about the popup method*/
		static FPopupMethodReply un_handle()
		{
			return FPopupMethodReply(false, EPopupMethod::CreateNewWindow);
		}

		/*create a reply that specifies how a popup should be handled*/
		static FPopupMethodReply use_method(EPopupMethod with_method)
		{
			return FPopupMethodReply(true, with_method);
		}

		/*which method to use for the popup, new window or reuse current window*/
		EPopupMethod get_popup_method() const
		{
			return m_popup_method;
		}

		/*should we throttle the engine?*/
		EShouldThrottle get_should_throttle() const
		{
			return m_should_throttle;
		}

		FPopupMethodReply()
			: TReplyBase<FPopupMethodReply>(false)
			, m_popup_method(EPopupMethod::CreateNewWindow)
			, m_should_throttle(EShouldThrottle::Yes)
		{}

		FPopupMethodReply(bool b_is_handled, EPopupMethod in_method)
		: TReplyBase<FPopupMethodReply>(false)
		, m_popup_method(in_method)
		, m_should_throttle(EShouldThrottle::Yes)
		{}

		EPopupMethod m_popup_method;
		EShouldThrottle m_should_throttle;
	};
}