#include <PreCompileHeader.h>

#include "SlateDrawBuffer.h"

#include "DrawElements.h"//FSlateWindowElementList

namespace DoDo {
	FSlateDrawBuffer::FSlateDrawBuffer()
	{
	}

	FSlateDrawBuffer::~FSlateDrawBuffer()
	{
	}

	void FSlateDrawBuffer::clear_buffer()
	{
		//remove any window elements that are no longer valid
		for(int32_t window_index = 0; window_index < m_window_element_lists_pool.size(); ++window_index)
		{
			//get paint window
			//window element lists will draw elements on paint window
			if(m_window_element_lists_pool[window_index]->get_paint_window() == nullptr)
			{
				auto first = m_window_element_lists_pool.begin() + window_index;
				auto second = m_window_element_lists_pool.end() - 1;
				std::iter_swap(first, second);//swap and remove
				m_window_element_lists_pool.pop_back();//pop last element
				--window_index;
			}
		}

		//move all the window elements back into the pool
		for(std::shared_ptr<FSlateWindowElementList> existing_list : m_window_element_lists)
		{
			if(existing_list->get_paint_window() != nullptr)
			{
				m_window_element_lists_pool.push_back(existing_list);
			}
		}

		//clear current window element list
		m_window_element_lists.clear();
	}

	FSlateWindowElementList& FSlateDrawBuffer::add_window_element_list(std::shared_ptr<SWindow> for_window)
	{
		for(int32_t window_index = 0; window_index < m_window_element_lists_pool.size(); ++window_index)
		{
			std::shared_ptr<FSlateWindowElementList> existing_element_list = m_window_element_lists_pool[window_index];

			if(existing_element_list->get_paint_window() == for_window.get())
			{
				m_window_element_lists.push_back(existing_element_list);

				auto first = m_window_element_lists_pool.begin() + window_index;
				auto second = m_window_element_lists_pool.end() - 1;

				std::iter_swap(first, second);//swap two iterators

				m_window_element_lists_pool.pop_back();

				//clear data
				existing_element_list->reset_element_list();

				return *existing_element_list;
			}
		}

		std::shared_ptr<FSlateWindowElementList> window_elements = std::make_shared<FSlateWindowElementList>(for_window);
		//todo:implement FSlateWindowElementList's ResetElementList
		window_elements->reset_element_list();
		m_window_element_lists.push_back(window_elements);

		return *window_elements;
	}
	void FSlateDrawBuffer::remove_unused_window_element(const std::vector<SWindow*>& all_windows)
	{
	}
}