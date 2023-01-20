#pragma once

#include "Core/Core.h"

namespace DoDo {
	class SWidget;
	/*
	* base class for all the ways that a slate widget can reply to events
	* typical replies include requests to set focus, capture or release the pointer
	* signify a cursor preference, etc
	*/
	class FReplyBase
	{
	public:
		/* return true if this reply is a result of the event being handled, false otherwise */
		bool is_event_handled() const { return m_b_is_handled; }

		/* the widget that ultimately handled the event */
		const std::shared_ptr<SWidget> get_handler() { return m_event_handler; }
	protected:

		/*a reply can be handled or unhandled, any widget handling event decides whether it has handled the event*/
		FReplyBase(bool in_is_handled)
			: m_b_is_handled(in_is_handled)
		{

		}

		/*has a widget handled on event*/
		bool m_b_is_handled;

		/*widget that handled the event that generated this reply*/
		std::shared_ptr<SWidget> m_event_handler;
	};

	template<typename ReplyType>
	class TReplyBase : public FReplyBase
	{
	public:
		TReplyBase(bool b_is_handled)
		: FReplyBase(m_b_is_handled)
		{
			
		}

	protected:
		//todo:implement FEventRouter

		/*set the widget that handled the event, undefined if never handled, this method is to be used by slate application only*/
		ReplyType* set_handler(const std::shared_ptr<SWidget>& in_handler)
		{
			this->m_event_handler = in_handler;
			return Me();
		}

		/*return a reference to this reply*/
		ReplyType& Me()
		{
			return static_cast<ReplyType&>(*this);
		}
	};

	/*a reply type for events that return a void reply, e.g. OnMouseLeave()*/
	class FNoReply : public TReplyBase<FNoReply>
	{
	public:
		static FNoReply un_handled() { return FNoReply(); }//FNoReply

		FNoReply() : TReplyBase<FNoReply>(false) {}//initialize reply to false
	};
}