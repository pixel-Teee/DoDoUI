#pragma once

#include <memory>

#include "ApplicationCore/GenericPlatform/ICursor.h"

#include "glm/glm.hpp"

#include <map>

#include "SlateCore/Layout/WidgetPath.h"
#include "SlateCore/Layout/WidgetPath.h"//FWidget depends on it

namespace DoDo
{
	/*
	 * slate's representation of an individual input-providing user
	 * as new input sources are connected, new slate users are created
	 */
	class FSlateUser : public std::enable_shared_from_this<FSlateUser>
	{
	public:
		virtual ~FSlateUser();

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

		void notify_pointer_move_begin(const FPointerEvent& pointer_event);

		void notify_pointer_move_complete(const FPointerEvent& pointer_event, const FWidgetPath& widgets_under_pointer);

		FWeakWidgetPath get_last_widgets_under_pointer(uint32_t pointer_index) const;

		static std::shared_ptr<FSlateUser> Create(int32_t in_user_index, std::shared_ptr<ICursor> in_cursor);

		FSlateUser(int32_t in_user_index, std::shared_ptr<ICursor> in_cursor);//todo:move to private scope

		//todo:implement this function
		FWidgetPath get_captor_path(uint32_t pointer_index, FWeakWidgetPath::EInterruptedPathHandling::Type interrupted_path_handling = FWeakWidgetPath::EInterruptedPathHandling::Truncate, const FPointerEvent* pointer_event = nullptr);

	private:
		
		void update_pointer_position(uint32_t pointer_index, const glm::vec2& position);
		/*the index the user was assigned*/
		int32_t m_user_index = -1;

		/*the cursor this user is in control of, guaranteed to be valid for all real users, absence implies this is a virtual user*/
		//todo:implement ICursor
		std::shared_ptr<ICursor> m_cursor;

		/*current position of all pointers controlled by this user*/
		std::map<uint32_t, glm::vec2> m_pointer_positions_by_index;
		std::map<uint32_t, glm::vec2> m_previous_pointer_position_by_index;

		//note:what it is?
		/*weak paths to widgets that are currently capturing a particular pointer*/
		std::map<uint32_t, FWeakWidgetPath> m_pointer_captor_paths_by_index;

		/*weak paths to the last widget each pointer was under last time an event was processed*/
		std::map<uint32_t, FWeakWidgetPath> m_widgets_under_pointer_last_event_by_index;
	};
}
