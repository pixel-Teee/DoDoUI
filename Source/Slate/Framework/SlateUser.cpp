#include <PreCompileHeader.h>

#include "SlateUser.h"

#include "Application/Application.h"

namespace DoDo
{
	FSlateUser::~FSlateUser()
	{
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
		if(it != m_previous_pointer_position_by_index.end())
		{
			it->second = position;
		}
	}
}
