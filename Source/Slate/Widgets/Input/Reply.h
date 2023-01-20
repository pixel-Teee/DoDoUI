#pragma once

#include "ReplyBase.h"//reply base

namespace DoDo {
	/*
	* a reply is something that a slate event returns to the system to notify it about certain aspect of how an event was handled
	* for example, a widget may handle an OnMouseDown event by asking the system to give mouse capture to a specific widget
	* to do this, return FReply::CaptureMpuse(NewMouseCapture)
	*/
	class FReply : public TReplyBase<FReply>
	{
	public:
		/*an event should return a FReply::Handled().CaptureMouse(SomeWidget) as a means of asking the system to forward all mouse events to SomeWidget*/
		FReply& capture_mouse(std::shared_ptr<SWidget> in_mouse_captor)
		{
			this->m_mouse_captor = in_mouse_captor;

			return Me();//return most derived type
		}

	public:
		/*
		* an event should return a FReply::Handled() to let the system know that an event was handled
		*/
		static FReply handled()
		{
			return FReply(true);//already handled
		}

	private:
		/*
		* hidden default constructor
		*/
		FReply(bool b_Is_Handled)
			: TReplyBase<FReply>(b_Is_Handled)
		{
			//todo:m_mouse_captor is nullptr
		}

		std::weak_ptr<SWidget> m_mouse_captor;
	};
}