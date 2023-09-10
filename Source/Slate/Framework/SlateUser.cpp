#include <PreCompileHeader.h>

#include "SlateUser.h"

#include "Application/Application.h"

#include "SlateCore/Input/CursorReply.h"//FCursorReply depends on it

#include "SlateCore/Widgets/SWidget.h"//SWidget depends on it

#include "SlateCore/Widgets/SWindow.h"

#include "SlateCore/Input/DragAndDrop.h"//FDragDropOperation depends on it

namespace DoDo
{
	FSlateUser::~FSlateUser()
	{
	}

	std::shared_ptr<SWidget> FSlateUser::get_focused_widget() const
	{
		return m_weak_focus_path.is_valid() ? m_weak_focus_path.get_last_widget().lock() : nullptr;
	}

	bool FSlateUser::set_focus(const std::shared_ptr<SWidget>& widget_to_focus, EFocusCause reason_focus_is_changing)
	{
		return Application::get().set_user_focus(m_user_index, widget_to_focus, reason_focus_is_changing);
	}

	void FSlateUser::clear_focus(EFocusCause reason_focus_is_changing)
	{
		Application::get().clear_user_focus(m_user_index, reason_focus_is_changing);
	}

	void FSlateUser::set_focus_path(const FWidgetPath& new_focus_path, EFocusCause in_focus_cause, bool b_in_show_focus)
	{
		//todo:add more logic
		m_weak_focus_path = new_focus_path;
		m_focus_cause = in_focus_cause;
		
	}

	std::optional<EFocusCause> FSlateUser::has_focus(std::shared_ptr<const SWidget> widget) const
	{
		return widget && get_focused_widget() == widget ? m_focus_cause : std::optional<EFocusCause>();
	}

	bool FSlateUser::has_any_capture() const
	{
		return m_pointer_captor_paths_by_index.size() > 0;
	}

	bool FSlateUser::has_cursor_capture() const
	{
		return has_capture(Application::m_cursor_pointer_index);
	}

	bool FSlateUser::has_capture(uint32_t pointer_index) const
	{
		auto it = m_pointer_captor_paths_by_index.find(pointer_index);

		if (it != m_pointer_captor_paths_by_index.end())
		{
			const FWeakWidgetPath* captor_path = &(it->second);
			return captor_path && captor_path->is_valid();//is valid will check have widgets one least
		}
		return false;
	}

	bool FSlateUser::is_widget_directly_under_any_pointer(std::shared_ptr<const SWidget> widget) const
	{
		for (const auto& index_path_pair : m_widgets_under_pointer_last_event_by_index)
		{
			if (index_path_pair.second.is_valid() && index_path_pair.second.get_last_widget().lock() == widget)
			{
				return true;
			}
		}

		return false;
	}

	glm::vec2 FSlateUser::get_cursor_position() const
	{
		return get_pointer_position(Application::m_cursor_pointer_index);
	}

	glm::vec2 FSlateUser::get_pointer_position(uint32_t pointer_index) const
	{
		//from ICursor to get position
		//if(m_cursor && pointer_index == Application::m_cursor_pointer_index)
		//{
		//	return m_cursor->get_position();
		//}

		//const glm::vec2* found_position =

		auto it = m_pointer_positions_by_index.find(pointer_index);

		if(it != m_pointer_positions_by_index.end())
		{
			const glm::vec2 found_position = it->second;
			return found_position;
		}
		else
		{
			return glm::vec2(0.0f, 0.0f);
		}
	}

	glm::vec2 FSlateUser::get_previous_cursor_position() const
	{
		return get_previous_pointer_position(Application::m_cursor_pointer_index);//note:this is identity cursor
	}

	glm::vec2 FSlateUser::get_previous_pointer_position() const
	{
		return get_previous_pointer_position(Application::m_cursor_pointer_index);
	}

	glm::vec2 FSlateUser::get_previous_pointer_position(uint32_t pointer_index) const
	{
		auto it = m_previous_pointer_position_by_index.find(pointer_index);

		if (it != m_previous_pointer_position_by_index.end())
		{
			const glm::vec2 found_position = it->second;
			return found_position;
		}
		else
		{
			return glm::vec2(0.0f, 0.0f);
		}
	}

	void FSlateUser::set_cursor_position(const glm::vec2 new_cursor_pos)
	{
		set_pointer_position(Application::m_cursor_pointer_index, new_cursor_pos.x, new_cursor_pos.y);
	}

	void FSlateUser::set_cursor_position(int32_t pos_x, int32_t pos_y)
	{
		set_pointer_position(Application::m_cursor_pointer_index, pos_x, pos_y);
	}

	void FSlateUser::set_pointer_position(uint32_t pointer_index, int32_t pos_x, int32_t pos_y)
	{
		if(m_cursor && pointer_index == Application::m_cursor_pointer_index)
		{
			m_cursor->set_position(pos_x, pos_y);
		}

		update_pointer_position(pointer_index, glm::vec2(pos_x, pos_y));//todo:store the cursor position
	}

	bool FSlateUser::does_widget_have_capture(std::shared_ptr<const SWidget> widget, uint32_t pointer_index) const
	{
		auto it = m_pointer_captor_paths_by_index.find(pointer_index);

		const FWeakWidgetPath* captor = nullptr;

		if (it != m_pointer_captor_paths_by_index.end())
		{
			captor = &(it->second);
		}

		return captor && captor->get_last_widget().lock() == widget;//get the weak widget path last widget
	}

	bool FSlateUser::does_widget_have_any_capture(std::shared_ptr<const SWidget> widget) const
	{
		for (const auto& index_path_pair : m_pointer_captor_paths_by_index)
		{
			if (index_path_pair.second.get_last_widget().lock() == widget)
			{
				return true;
			}
		}
		return false;
	}

	bool FSlateUser::set_cursor_captor(std::shared_ptr<const SWidget> widget, const FWidgetPath& event_path)
	{
		return set_pointer_captor(Application::m_cursor_pointer_index, widget, event_path);
	}

	bool FSlateUser::set_pointer_captor(uint32_t pointer_index, std::shared_ptr<const SWidget> widget, const FWidgetPath& event_path)
	{
		//todo:implement release capture

		FWidgetPath new_captor_path = event_path.get_path_down_to(widget);

		if (!new_captor_path.is_valid() || new_captor_path.get_last_widget() != widget)
		{
			//the target widget wasn't in the given event path, so try searching for it from the root of the event path
			new_captor_path = event_path.get_path_down_to(std::reinterpret_pointer_cast<SWidget>(event_path.get_window()));
			//todo:implement extend path to
		}

		if (new_captor_path.is_valid() && new_captor_path.get_last_widget() == widget)
		{
			m_pointer_captor_paths_by_index.insert({ pointer_index, new_captor_path });

			return true;
		}

		return false;
	}

	void FSlateUser::release_capture(uint32_t pointer_index)
	{
		auto it = m_pointer_captor_paths_by_index.find(pointer_index);
		
		const FWeakWidgetPath* captor_path = &(it->second);

		if (captor_path)
		{
			m_widgets_under_pointer_last_event_by_index.insert({ pointer_index, *captor_path });//note:what it is?, will remove at drag exit

			if (std::shared_ptr<SWidget> captor_width = captor_path->get_last_widget().lock())
			{
				//todo:implement on mouse capture lost
			}

			captor_path = nullptr;

			m_pointer_captor_paths_by_index.erase(pointer_index);

			//todo:implement request cursor query
			if (pointer_index == Application::m_cursor_pointer_index)
			{
				//if cursor capture changes, we should refresh the cursor state
				request_cursor_query();//reset to default cursor
			}
		}
	}

	void FSlateUser::update_cursor()
	{
		if (!m_cursor)
		{
			return;
		}

		if (m_locking_widget_path.is_valid())
		{
			const FWidgetPath path_to_widget = m_locking_widget_path.to_widget_path(FWeakWidgetPath::EInterruptedPathHandling::ReturnInvalid);

			if (path_to_widget.is_valid())
			{
				const FSlateRect computed_clip_rect = path_to_widget.m_widgets.last().m_geometry.get_layout_bounding_rect();

				if (computed_clip_rect != m_last_computed_lock_bounds)
				{
					//the locking widget is still valid, but its bounds have changed - gotta update the lock boundaries on the cursor to match
					//todo:implement lock cursor internal
				}
			}
			else
			{
				//todo:implement unlock cursor
			}
		}

		//when slate captures the mouse, it is up to us to set the cursor because the os assumes that we own the mouse
		if (has_any_capture() || m_b_query_cursor_requested) //todo:add b_query_cursor_requested
		{
			query_cursor();
		}
	}

	void FSlateUser::process_cursor_reply(const FCursorReply& cursor_reply)
	{
		if (m_cursor && cursor_reply.is_event_handled()) //note:when cursor reply event reply is handled, the cursor is set
		{
			//todo:add can draw cursor handle

			m_cursor->set_type(EMouseCursor::None);
		}
		else
		{
			m_cursor_window_ptr.reset();
			m_cursor_widget_ptr.reset();
		}
	}

	void FSlateUser::process_cursor_reply(const std::shared_ptr<SWindow>& slate_window, const FCursorReply& cursor_reply)
	{
		if (m_cursor && cursor_reply.is_event_handled()) //note:when cursor reply event reply is handled, the cursor is set
		{
			//todo:add can draw cursor handle

			if (slate_window)
			{
				std::shared_ptr<Window> window = slate_window->get_native_window();
				m_cursor->set_type(window, cursor_reply.get_cursor_type());
			}
		}
		else
		{
			m_cursor_window_ptr.reset();
			m_cursor_widget_ptr.reset();
		}
	}

	void FSlateUser::query_cursor()
	{
		m_b_query_cursor_requested = false;

		if (m_cursor) //todo:add more cursor
		{
			Application& slate_app = Application::get();

			FCursorReply cursor_reply = FCursorReply::un_handled();

			//todo:add drag drop content handle

			FWidgetPath widgets_to_query_for_cursor;//todo:move to there
			if (!cursor_reply.is_event_handled())
			{
				const bool b_has_hard_ware_cursor = slate_app.get_platform_cursor() == m_cursor;//ICursor class, platform application will store the cursor
				
				const glm::vec2 current_cursor_position = get_cursor_position();

				const glm::vec2 last_cursor_position = get_previous_cursor_position();

				const std::set<FKey> empty_set;
				const FPointerEvent cursor_event(
					0,//todo:fix me, add get user index for mouse
					Application::m_cursor_pointer_index,
					current_cursor_position,
					last_cursor_position,
					current_cursor_position - last_cursor_position,
					b_has_hard_ware_cursor ? slate_app.get_pressed_mouse_buttons() : empty_set, //note:from platform application to get mouse button and modifier state
					b_has_hard_ware_cursor ? slate_app.get_modifier_keys() : FModifierKeyState()
					);

				
				if (has_cursor_capture())
				{
					//query widgets with mouse capture for the cursor
					FWidgetPath mouse_captor_path = get_cursor_captor_path(FWeakWidgetPath::EInterruptedPathHandling::Truncate, &cursor_event);

					if (mouse_captor_path.is_valid())
					{
						std::shared_ptr<SWindow> capture_window = mouse_captor_path.get_window();
						const std::shared_ptr<SWindow> active_modal_window = nullptr;//todo:implement slate application get active modal window

						//never query the mouse captor path if it is outside an active modal window
						if (!active_modal_window || capture_window == active_modal_window) //todo:check capture window is descendant of active modal window
						{
							widgets_to_query_for_cursor = mouse_captor_path;
						}
					}
				}
				else
				{
					widgets_to_query_for_cursor = slate_app.locate_window_under_mouse(current_cursor_position, slate_app.get_interactive_top_level_windows(),false, m_user_index);
				}

				if (widgets_to_query_for_cursor.is_valid())
				{
					for (int32_t widget_index = widgets_to_query_for_cursor.m_widgets.num() - 1; widget_index >= 0; --widget_index)
					{
						const FArrangedWidget& arranged_widget = widgets_to_query_for_cursor.m_widgets[widget_index];

						cursor_reply = arranged_widget.m_widget->On_Cursor_Query(arranged_widget.m_geometry, cursor_event);//note:from meta data to get cursor type and construct reply to return

						if (cursor_reply.is_event_handled())
						{
							if (!cursor_reply.get_cursor_widget())
							{
								//for (; widget_index >= 0; --widget_index)
								//{
								//	std::optional<std::shared_ptr<SWidget>> cursor_widget = widgets_to_query_for_cursor.m_widgets[widget_index].m_widget->
								//}

								//todo:add on map cursor
							}
						}
						break;
					}

					if (!cursor_reply.is_event_handled() && widgets_to_query_for_cursor.is_valid())
					{
						//query was not handled, and we are still over a slate window
						cursor_reply = FCursorReply::Cursor(EMouseCursor::Default);
					}
				}
				else
				{
					//set the default cursor when there isn't an active window under the cursor and the mouse isn't captured
					cursor_reply = FCursorReply::Cursor(EMouseCursor::Default);
				}
			}
			process_cursor_reply(widgets_to_query_for_cursor.get_window(), cursor_reply);
		}
	}

	void FSlateUser::notify_pointer_move_begin(const FPointerEvent& pointer_event)
	{
		auto it = m_pointer_positions_by_index.find(pointer_event.get_pointer_index());

		if (it == m_pointer_positions_by_index.end())
		{
			auto it2 = m_pointer_positions_by_index.insert({ pointer_event.get_pointer_index(), glm::vec2(0.0f, 0.0f) });
			it2.first->second = pointer_event.get_screen_space_position();
		}
		else
		{
			it->second = pointer_event.get_screen_space_position();
		}
	}

	void FSlateUser::notify_pointer_move_complete(const FPointerEvent& pointer_event, const FWidgetPath& widgets_under_pointer)
	{
		//m_previous_pointer_position_by_index.insert({ pointer_event.get_pointer_index(), pointer_event.get_screen_space_position() });

		if (is_drag_dropping_affected(pointer_event))
		{
			FDragDropEvent drag_drop_event(pointer_event, m_drag_drop_content);

			m_drag_drop_content->On_Dragged(drag_drop_event);

			//update the window we're under for rendering the drag drop operation if it's a windowless drag drop operation
			if (widgets_under_pointer.is_valid())
			{
				//todo:get window
			}
			else
			{

			}
		}

		auto it = m_previous_pointer_position_by_index.find(pointer_event.get_pointer_index());

		if (it != m_previous_pointer_position_by_index.end())
		{
			it->second = pointer_event.get_screen_space_position();
			//std::cout << "notify pointer move complete pointer event (" << pointer_event.get_screen_space_position().x << ", " << pointer_event.get_screen_space_position().y << ")" << std::endl;
		}
		else
		{
			m_previous_pointer_position_by_index.insert({ pointer_event.get_pointer_index(), pointer_event.get_screen_space_position() });
			//std::cout << m_previous_pointer_position_by_index.size() << std::endl;
		}

		auto it2 = m_widgets_under_pointer_last_event_by_index.find(pointer_event.get_pointer_index());

		if (it2 != m_widgets_under_pointer_last_event_by_index.end())
		{
			it2->second = FWeakWidgetPath(widgets_under_pointer);
		}
		else
		{
			m_widgets_under_pointer_last_event_by_index.insert({ pointer_event.get_pointer_index(), FWeakWidgetPath(widgets_under_pointer) });
		}

	}

	void FSlateUser::notify_pointer_released(const FPointerEvent& pointer_event, const FWidgetPath& widgets_under_cursor, std::shared_ptr<FDragDropOperation> dropped_content, bool b_was_handled)
	{
		const int32_t pointer_index = pointer_event.get_pointer_index();

		auto drag_state = m_drag_states_by_pointer_index.find(pointer_index);

		if (drag_state != m_drag_states_by_pointer_index.end() && drag_state->second.m_trigger_button == pointer_event.get_effecting_button())
		{
			//the user has released the button (or finger) that was supposed to start the drag, stop detecting it
			
			m_drag_states_by_pointer_index.erase(pointer_index);
		}

		if (!has_capture(pointer_index))
		{
			//todo:when we perform a touch end, we need to also send a mouse leave as if it were a cursor

			if (dropped_content && dropped_content->affected_by_pointer_event(pointer_event))
			{
				FPointerEvent modified_event(pointer_event);
				modified_event.set_event_path(widgets_under_cursor);
				dropped_content->On_Drop(b_was_handled, modified_event);
				m_widgets_under_pointer_last_event_by_index.erase(pointer_index);
			}
		}

		//todo:add more logic
	}

	void FSlateUser::set_drag_drop_content(std::shared_ptr<FDragDropOperation> in_drag_drop_content)
	{
		m_drag_drop_content = in_drag_drop_content;
	}

	void FSlateUser::reset_drag_drop_content()
	{
		m_drag_drop_content.reset();
	}

	bool FSlateUser::is_drag_dropping() const
	{
		return m_drag_drop_content != nullptr;//check
	}

	bool FSlateUser::is_drag_dropping_affected(const FPointerEvent& in_pointer_event) const
	{
		return m_drag_drop_content && m_drag_drop_content->affected_by_pointer_event(in_pointer_event);
	}

	void FSlateUser::start_drag_detection(const FWidgetPath& path_to_widget, int32_t pointer_index, FKey drag_button, glm::vec2 start_location)
	{
		m_drag_states_by_pointer_index.insert({ pointer_index, FDragDetectionState(path_to_widget, pointer_index, drag_button, start_location) });
	}

	FWidgetPath FSlateUser::detect_drag(const FPointerEvent& pointer_event, float drag_trigger_distance)
	{
		auto it = m_drag_states_by_pointer_index.find(pointer_event.get_pointer_index());

		if(it != m_drag_states_by_pointer_index.end())
		{
			FDragDetectionState& drag_state = it->second;

			const glm::vec2 drag_delta = drag_state.m_drag_start_location - pointer_event.get_screen_space_position();

			float drag_delta_size_squared = drag_delta.x * drag_delta.x + drag_delta.y * drag_delta.y;

			float drag_trigger_distance_size_squared = drag_trigger_distance * drag_trigger_distance;

			if(drag_delta_size_squared > drag_trigger_distance_size_squared)
			{
				//weak path to widget path (add geometry)
				FWidgetPath drag_detection_path = drag_state.m_detect_drag_for_widget.to_widget_path(FWeakWidgetPath::EInterruptedPathHandling::ReturnInvalid);

				if(drag_detection_path.is_valid())
				{
					reset_drag_detection();
					return drag_detection_path;
				}
			}
		}

		return FWidgetPath();
	}

	void FSlateUser::reset_drag_detection()
	{
		m_drag_states_by_pointer_index.clear();
	}

	FWeakWidgetPath FSlateUser::get_last_widgets_under_pointer(uint32_t pointer_index) const
	{
		auto it = m_widgets_under_pointer_last_event_by_index.find(pointer_index);

		if (it != m_widgets_under_pointer_last_event_by_index.end())
		{
			return it->second;
		}

		return FWeakWidgetPath();//null
	}

	std::shared_ptr<FSlateUser> FSlateUser::Create(int32_t in_user_index, std::shared_ptr<ICursor> in_cursor)
	{
		return std::make_shared<FSlateUser>(in_user_index, in_cursor);
	}

	FSlateUser::FSlateUser(int32_t in_user_index, std::shared_ptr<ICursor> in_cursor)
		: m_user_index(in_user_index)
		, m_cursor(in_cursor)
	{
		m_pointer_positions_by_index.insert({ Application::m_cursor_pointer_index, glm::vec2(0.0f, 0.0f) });
		m_previous_pointer_position_by_index.insert({ Application::m_cursor_pointer_index, glm::vec2(0.0f, 0.0f) });
	}

	FWidgetPath FSlateUser::get_captor_path(uint32_t pointer_index,
		FWeakWidgetPath::EInterruptedPathHandling::Type interrupted_path_handling, const FPointerEvent* pointer_event)
	{
		FWidgetPath captor_path;

		auto it = m_pointer_captor_paths_by_index.find(pointer_index);

		if(it != m_pointer_captor_paths_by_index.end())
		{
			FWeakWidgetPath* weak_captor_path = &(it->second);

			if(weak_captor_path->to_widget_path(captor_path, interrupted_path_handling, pointer_event) == FWeakWidgetPath::EPathResolutionResult::Truncated)
			{
				//the path was truncated, meaning it's not actually valid anymore, so we want clear out entry for it out immediately
				weak_captor_path = nullptr;
				//todo:implement release capture
			}
		}
		return captor_path;
	}

	FWidgetPath FSlateUser::get_cursor_captor_path(FWeakWidgetPath::EInterruptedPathHandling::Type interrupted_path_handling, const FPointerEvent* pointer_event)
	{
		return get_captor_path(Application::m_cursor_pointer_index, interrupted_path_handling, pointer_event);
	}

	void FSlateUser::update_pointer_position(uint32_t pointer_index, const glm::vec2& position)
	{
		auto it = m_pointer_positions_by_index.find(pointer_index);
		if(it != m_pointer_positions_by_index.end())
		{
			it->second = position;
		}

		it = m_previous_pointer_position_by_index.find(pointer_index);
		if (it != m_previous_pointer_position_by_index.end())
		{
			it->second = position;
		}

		//todo:fix me
	}
}
