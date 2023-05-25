#pragma once

#include <memory>

#include "ApplicationCore/GenericPlatform/ICursor.h"

#include "glm/glm.hpp"

#include <map>

#include "SlateCore/Layout/WidgetPath.h"//FWidget depends on it

namespace DoDo
{
	class FCursorReply;
	class FDragDropOperation;
	/*
	 * slate's representation of an individual input-providing user
	 * as new input sources are connected, new slate users are created
	 */
	class FSlateUser : public std::enable_shared_from_this<FSlateUser>
	{
	public:
		virtual ~FSlateUser();

		bool set_focus(const std::shared_ptr<SWidget>& widget_to_focus, EFocusCause reason_focus_is_changing = EFocusCause::SetDirectly);

		void clear_focus(EFocusCause reason_focus_is_changing = EFocusCause::SetDirectly);

		void set_focus_path(const FWidgetPath& new_focus_path, EFocusCause in_focus_casue, bool b_in_show_focus);

		bool has_any_capture() const;

		bool has_cursor_capture() const;

		bool has_capture(uint32_t pointer_index) const;

		int32_t get_user_index() const { return m_user_index; }

		glm::vec2 get_cursor_position() const;

		glm::vec2 get_pointer_position(uint32_t pointer_index) const;

		glm::vec2 get_previous_cursor_position() const;

		glm::vec2 get_previous_pointer_position() const;

		glm::vec2 get_previous_pointer_position(uint32_t pointer_index) const;

		void set_cursor_position(const glm::vec2 new_cursor_pos);

		void set_cursor_position(int32_t pos_x, int32_t pos_y);

		void set_pointer_position(uint32_t pointer_index, int32_t pos_x, int32_t pos_y);

		bool does_widget_have_capture(std::shared_ptr<const SWidget> widget, uint32_t pointer_index) const;

		bool does_widget_have_any_capture(std::shared_ptr<const SWidget> widget) const;

		bool set_cursor_captor(std::shared_ptr<const SWidget> widget, const FWidgetPath& event_path);

		bool set_pointer_captor(uint32_t pointer_index, std::shared_ptr<const SWidget> widget, const FWidgetPath& event_path);

		void release_capture(uint32_t pointer_index);

		void update_cursor();

		void request_cursor_query() { m_b_query_cursor_requested = true; }

		void process_cursor_reply(const FCursorReply& cursor_reply);

		void process_cursor_reply(const std::shared_ptr<SWindow>& slate_window, const FCursorReply& cursor_reply);//note:use for glfw

		void query_cursor();

		void notify_pointer_move_begin(const FPointerEvent& pointer_event);

		void notify_pointer_move_complete(const FPointerEvent& pointer_event, const FWidgetPath& widgets_under_pointer);

		void notify_pointer_released(const FPointerEvent& pointer_event, const FWidgetPath& widgets_under_cursor, std::shared_ptr<FDragDropOperation> dropped_content,
			bool b_was_handled);

		void set_drag_drop_content(std::shared_ptr<FDragDropOperation> in_drag_drop_content);
		void reset_drag_drop_content();

		std::shared_ptr<FDragDropOperation> get_drag_drop_content() const { return m_drag_drop_content; }
		bool is_drag_dropping() const;
		bool is_drag_dropping_affected(const FPointerEvent& in_pointer_event) const;

		void start_drag_detection(const FWidgetPath& path_to_widget, int32_t pointer_index, FKey drag_button, glm::vec2 start_location);

		FWidgetPath detect_drag(const FPointerEvent& pointer_event, float drag_trigger_distance);
		void reset_drag_detection();

		FWeakWidgetPath get_last_widgets_under_pointer(uint32_t pointer_index) const;
		const std::map<uint32_t, FWeakWidgetPath>& get_widgets_under_pointer_last_event_by_index() const { return m_widgets_under_pointer_last_event_by_index; }

		static std::shared_ptr<FSlateUser> Create(int32_t in_user_index, std::shared_ptr<ICursor> in_cursor);

		std::shared_ptr<FWidgetPath> get_focus_path() const
		{
			if (!m_strong_focus_path)
			{
				m_strong_focus_path = m_weak_focus_path.to_widget_path_ptr();
			}

			return m_strong_focus_path;
		}

		FSlateUser(int32_t in_user_index, std::shared_ptr<ICursor> in_cursor);//todo:move to private scope

		//todo:implement this function
		FWidgetPath get_captor_path(uint32_t pointer_index, FWeakWidgetPath::EInterruptedPathHandling::Type interrupted_path_handling = FWeakWidgetPath::EInterruptedPathHandling::Truncate, const FPointerEvent* pointer_event = nullptr);
		FWidgetPath get_cursor_captor_path(FWeakWidgetPath::EInterruptedPathHandling::Type interrupted_path_handling = FWeakWidgetPath::EInterruptedPathHandling::Truncate,
			const FPointerEvent* pointer_event = nullptr);

		const FWeakWidgetPath& get_weak_focus_path() const { return m_weak_focus_path; }
	private:
		
		void update_pointer_position(uint32_t pointer_index, const glm::vec2& position);
		/*the index the user was assigned*/
		int32_t m_user_index = -1;

		/*the cursor this user is in control of, guaranteed to be valid for all real users, absence implies this is a virtual user*/
		//todo:implement ICursor
		std::shared_ptr<ICursor> m_cursor;

		/*the cursor widget and window to render that cursor for the current software cursor*/
		std::weak_ptr<SWindow> m_cursor_window_ptr;
		std::weak_ptr<SWidget> m_cursor_widget_ptr;

		/*a weak path to widget currently focused by a user, if any*/
		FWeakWidgetPath m_weak_focus_path;

		/*a strong widget path to the focused widget, if any, this is cleared after the end of pumping messages*/
		mutable std::shared_ptr<FWidgetPath> m_strong_focus_path;

		/*the os or actions taken by the user may require we refresh the current state of the cursor*/
		bool m_b_query_cursor_requested = false;

		/*current position of all pointers controlled by this user*/
		std::map<uint32_t, glm::vec2> m_pointer_positions_by_index;
		std::map<uint32_t, glm::vec2> m_previous_pointer_position_by_index;

		//note:what it is?
		/*weak paths to widgets that are currently capturing a particular pointer*/
		std::map<uint32_t, FWeakWidgetPath> m_pointer_captor_paths_by_index;

		struct FDragDetectionState
		{
			FDragDetectionState(const FWidgetPath& path_to_widget, int32_t pointer_index, FKey drag_button, const glm::vec2& start_location)
				: m_detect_drag_for_widget(path_to_widget)
				, m_drag_start_location(start_location)
				, m_trigger_button(drag_button)
				, m_pointer_index(pointer_index)
			{}
			/*if not null, a widget has requsted that we detect a drag being triggered in this widget and send an OnDragDected() event*/
			FWeakWidgetPath m_detect_drag_for_widget;

			glm::vec2 m_drag_start_location = glm::vec2(0.0f);

			FKey m_trigger_button = EKeys::Invalid;

			int32_t m_pointer_index = -1;
		};

		/*current drag status for pointers currently executing a drag/drop operation*/
		std::map<uint32_t, FDragDetectionState> m_drag_states_by_pointer_index;

		/*when not null, the content of the current drag drop operation*/
		std::shared_ptr<FDragDropOperation> m_drag_drop_content;

		/*weak paths to the last widget each pointer was under last time an event was processed*/
		std::map<uint32_t, FWeakWidgetPath> m_widgets_under_pointer_last_event_by_index;

		/*path to widget that currently holds the cursor lock, invalid path if no cursor lock*/
		FWeakWidgetPath m_locking_widget_path;

		/*desktop space rect that bounds the cursor*/
		FSlateRect m_last_computed_lock_bounds;
	};
}
