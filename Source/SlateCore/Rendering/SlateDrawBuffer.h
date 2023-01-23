#pragma once

#include "Core/Core.h"

#include <vector>

#include "glm/glm.hpp"//glm::vec2

namespace DoDo {
	class FSlateWindowElementList;
	class SWindow;
	/*
	* implements a draw buffer for slate
	*/
	class FSlateDrawBuffer
	{
	public:
		/*default constructor*/
		FSlateDrawBuffer();

		/*removes all data from the buffer*/
		void clear_buffer();

		/*
		* creates a new FSlateWindowElementList and returns a reference to it so it can have draw elements added to it
		*/
		FSlateWindowElementList& add_window_element_list(std::shared_ptr<SWindow> for_window);

		/*removes any window from the draw buffer that's not in this list or whose window has become invalid*/
		void remove_unused_window_element(const std::vector<SWindow*>& all_windows);


		/*
		* gets all window element lists in this buffer
		*/
		const std::vector<std::shared_ptr<FSlateWindowElementList>>& get_window_element_lists()
		{
			return m_window_element_lists;
		}

	private:
		//list of window element lists
		std::vector<std::shared_ptr<FSlateWindowElementList>> m_window_element_lists;

		glm::vec2 m_view_offset;
	};
}