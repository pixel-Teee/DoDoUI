#pragma once

#include "ReplyBase.h"

#include "ApplicationCore/GenericPlatform/ICursor.h"//EMouseCursor::Type depends on it

namespace DoDo {
	class SWindow;
	/*
	* a reply to the OnQueryCursor event
	*/
	class FCursorReply : public TReplyBase<FCursorReply>
	{
	public:
		/*
		* makes a null response meaning no preferences
		* i.e. if your widget returns this, its parent will get to decide what the cursor should be
		* this is the default behavior for a widget
		*/
		static FCursorReply un_handled()
		{
			return FCursorReply();
		}

		/*
		* respond with a specific cursor
		* this cursor will be used and no other widgets will be asked
		*/
		static FCursorReply Cursor(EMouseCursor::Type in_cursor)
		{
			return FCursorReply(in_cursor);
		}

		/*@return the custom cursor widget to render if set and the event was handled*/
		std::shared_ptr<SWidget> get_cursor_widget() const { return m_cursor_widget; }

		/*@return the requested mouse cursor if no custom widget is set and the event was handled*/
		EMouseCursor::Type get_cursor_type() const { return m_mouse_cursor; }

	private:
		/*internal constructor - makes a null result*/
		FCursorReply()
			: TReplyBase<FCursorReply>(false)
			, m_mouse_cursor(EMouseCursor::Default)
		{}

		/*internal constructor - makes a non-Null result*/
		FCursorReply(EMouseCursor::Type in_cursor_type)
			: TReplyBase<FCursorReply>(true)
			, m_mouse_cursor(in_cursor_type)
		{}

		/*window to render for cursor*/
		std::shared_ptr<SWindow> m_cursor_window;

		/*custom widget to render for cursor*/
		std::shared_ptr<SWidget> m_cursor_widget;

		/*the cursor type must be set is cursor widget is invalid*/
		EMouseCursor::Type m_mouse_cursor;
	};
}