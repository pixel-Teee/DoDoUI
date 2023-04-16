#pragma once

#include <memory>

#include "ApplicationCore/GenericPlatform/ICursor.h"

#include "SlateCore/Input/Events.h"//FDragDropEvent depends on it

namespace DoDo {
	class DoDoUtf8String;
	class SWindow;
	class SWidget;
	/*
	* a base class for a DragAndDrop operation which supports reflection
	* drag and drop is inherently quite stateful
	* implementing a custom DragDropOperation allows for dedicated handling of
	* a drag drop operation which keeps a decorator window (optionally)
	* implement derived types with DRAG_DROP_OPERATOR_TYPE (see below)
	*/
	class FDragDropOperation : public std::enable_shared_from_this<FDragDropOperation>
	{
	public:
		/*
		* default constructor
		*/
		FDragDropOperation();

		/*
		* destructor
		*/
		virtual ~FDragDropOperation();

		/*check if this drag and drop operation can cast safely to the specified template type*/
		template<class TType> bool is_of_type() const
		{
			return is_of_type_impl(TType::get_type_id());
		}

		/*checks whether this drag and drop operation can cast safely to the specified type*/
		virtual bool is_of_type_impl(const DoDoUtf8String& type) const
		{
			return false;
		}

		/*
		 * checks if this drag and drop operation is affected by a given pointer event
		 *
		 * @param pointer event the mouse event we are checking against
		 * @return true if the pointer event affects this drag and drop operation (default true)
		 */
		virtual bool affected_by_pointer_event(const FPointerEvent& pointer_event) { return true; }

		/*
		* gets the widget that will serve as the decorator unless overridden
		* if you do not override, you will have no decorator
		*/
		virtual std::shared_ptr<SWidget> get_default_decorator() const { return std::shared_ptr<SWidget>(); }

		/*
		* called when the mouse was moved during a drag and drop operation
		* 
		* @param DragDropEvent the event that describes this drag drop operation
		*/
		virtual void On_Dragged(const class FDragDropEvent& drag_drop_event);

		/*
		* invoked when the drag and drop operation has ended
		*
		* @param bDropWasHandled true when the drop was handled by some widget, false otherwise
		*/
		virtual void On_Drop(bool b_drop_was_handled, const FPointerEvent& mouse_event);
	protected:
		/*
		* constructs the window and widget if applicable
		*/
		virtual void Construct();

		/*
		* creates a window for the cursor decorator
		*/
		void create_cursor_decorator_window();

		/*
		* destroys the cursor decorator window
		*/
		void destroy_cursor_decorator_window();

		/*
		* create a new window for the decorator content, if this is false,
		* slate application will draw the decorator directly using the 
		*/
		bool m_b_create_new_window;

		/*the window that owns the decorator widget*/
		std::shared_ptr<SWindow> m_cursor_decorator_window;

		/*mouse cursor used by the drag drop operation*/
		std::optional<EMouseCursor::Type> m_mouse_cursor;

		/*mouse cursor used to temporarily replace the drag drops cursor*/
		std::optional<EMouseCursor::Type> m_mouse_cursor_override;
	};

	/*
	* all drag drop operations that requires type checking must include this macro
	* example usage:
	* class FMyDragDropOp : public FDragDropOperation
	* {
	*	public:
	*		DRAG_DROP_OPERATOR_TYPE(FMyDragDropOp, FDragDropOperation)
	*		...
	* };
	*/
#define DRAG_DROP_OPERATE_TYPE(TYPE, BASE) \
	static const DoDoUtf8String& get_type_id() { static DoDoUtf8String Type = #TYPE; return Type; }\
	virtual bool is_of_type_impl(const DoDoUtf8String& Type) const override { return get_type_id() == Type || BASE::is_of_type_impl(Type); }

	/*like a mouse event but with content*/
	class FDragDropEvent : public FPointerEvent
	{
	public:
		/*
		* construct a drag drop event
		* 
		* @param InMouseEvent the mouse event that is causing this drag and drop event to occur
		* @param InContent the content being dragged
		*/
		FDragDropEvent(const FPointerEvent& in_mouse_event, const std::shared_ptr<FDragDropOperation> in_content)
			: FPointerEvent(in_mouse_event)
			, m_content(in_content)
		{}

		/*@return the content being dragged*/
		std::shared_ptr<FDragDropOperation> get_operation() const
		{
			return m_content;
		}

		/*@return the content being dragged if it matched the 'OperationType'; invalid ptr otherwise. */
		template<typename OperationType>
		std::shared_ptr<OperationType> get_operation_as() const;

	private:
		/*the content being dragged*/
		std::shared_ptr<FDragDropOperation> m_content;
	};

	#include "DragAndDrop.inl"
}