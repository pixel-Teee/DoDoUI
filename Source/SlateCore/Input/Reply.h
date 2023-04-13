#pragma once

#include "ReplyBase.h"//reply base

#include "Core/InputCore/InputCoreTypes.h"//FKey depends on it

namespace DoDo {
	/*
	* a reply is something that a slate event returns to the system to notify it about certain aspect of how an event was handled
	* for example, a widget may handle an OnMouseDown event by asking the system to give mouse capture to a specific widget
	* to do this, return FReply::CaptureMpuse(NewMouseCapture)
	*/
	class FDragDropOperation;
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

		/*
		* an event should return FReply::Handled().BeginDragDrop(Content) to initialize a drag and drop operation
		* 
		* @param InDragDropContent the content that is being dragged, this could be a widget, or some arbitrary data
		* 
		* @return reference back the FReply so that this call be chained
		*/
		FReply& begin_drag_drop(std::shared_ptr<FDragDropOperation> in_drag_drop_content)
		{
			this->m_drag_drop_content = in_drag_drop_content;
			return Me();
		}

		/*an event should return FReply::Handled().EndDragDrop() to request that the current drag/drop operation be terminated*/
		FReply& end_drag_drop()
		{
			this->m_b_end_drag_drop = true;
			return Me();
		}
		/*
		* ask slate to detect if a user started dragging in this widget
		* if a drag is detected, slate will send an OnDragDetected event
		* 
		* @param DetectDragInMe detect dragging in this widget
		* @param MouseButton this button should be pressed to detect the drag
		*/
		FReply& detect_drag(const std::shared_ptr<SWidget>& detect_drag_in_me, FKey mouse_button)
		{
			this->m_detect_drag_for_widget = detect_drag_in_me;
			this->m_detect_drag_for_mouse_button = mouse_button;
			return Me();
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

		/*@return the content that we should use for the drag and drop operation, invalid shared ptr if a drag and drop operation is not requested*/
		const std::shared_ptr<FDragDropOperation>& get_drag_drop_content() const { return m_drag_drop_content; }

		/*@return a widget for which to detect a drag, invalid shared ptr if no drag detection requested*/
		std::shared_ptr<SWidget> get_detect_drag_request() const { return m_detect_drag_for_widget.lock(); }

		/*@return the mouse button for which we are detecting a drag*/
		FKey get_detect_drag_request_button() const { return m_detect_drag_for_mouse_button; }
	private:
		/*
		* hidden default constructor
		*/
		FReply(bool b_Is_Handled)
			: TReplyBase<FReply>(b_Is_Handled)
			, m_b_release_mouse_capture(false)
			, m_b_end_drag_drop(false)
		{
			//todo:m_mouse_captor is nullptr
		}

		std::weak_ptr<SWidget> m_mouse_captor;
		std::weak_ptr<SWidget> m_detect_drag_for_widget;
		FKey m_detect_drag_for_mouse_button;
		std::shared_ptr<FDragDropOperation> m_drag_drop_content;
		uint32_t m_b_release_mouse_capture : 1;
		uint32_t m_b_end_drag_drop : 1;
	};
}