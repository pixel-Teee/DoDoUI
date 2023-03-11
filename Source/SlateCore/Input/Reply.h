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

		/*
		 * an event should return a FReply::Unhandled() to let the system know that an event was unhandled
		 */
		static FReply un_handled()
		{
			return FReply(false);
		}
	public:
		/*true if this reply indicated that we should release mouse capture as a result of the evnt being handled*/
		bool should_release_mouse() const { return m_b_release_mouse_capture; }

		/*if the event replied with a request to capture the mouse, this returns the desired mouse captor, otherwise returns an invalid pointer*/
		std::shared_ptr<SWidget> get_mouse_captor() const { return m_mouse_captor.lock(); }

		/*
		* an event should return a FReply::Handled().ReleaseMouse() to ask the system to release mouse capture
		* note: deactivates high precision mouse movement if activated
		*/
		FReply& release_mouse_capture()//todo:complete this function
		{
			this->m_mouse_captor.reset();//clear
			this->m_b_release_mouse_capture = true;
			return Me();
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
		uint32_t m_b_release_mouse_capture : 1;
	};
}