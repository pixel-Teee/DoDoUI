#include <PreCompileHeader.h>

#include "SlateUser.h"

#include "Application/Application.h"

namespace DoDo
{
	FSlateUser::~FSlateUser()
	{
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

	glm::vec2 FSlateUser::get_cursor_position() const
	{
		return get_pointer_position(Application::m_cursor_pointer_index);
	}

	glm::vec2 FSlateUser::get_pointer_position(uint32_t pointer_index) const
	{
		//from ICursor to get position

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
		m_previous_pointer_position_by_index.insert({ pointer_event.get_pointer_index(), pointer_event.get_screen_space_position() });
		m_widgets_under_pointer_last_event_by_index.insert({ pointer_event.get_pointer_index(), FWeakWidgetPath(widgets_under_pointer) });
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
