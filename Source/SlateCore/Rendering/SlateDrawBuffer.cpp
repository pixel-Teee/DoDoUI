#include <PreCompileHeader.h>

#include "SlateDrawBuffer.h"

#include "DrawElements.h"//FSlateWindowElementList

namespace DoDo {
	FSlateDrawBuffer::FSlateDrawBuffer()
	{
	}
	void FSlateDrawBuffer::clear_buffer()
	{
	}
	FSlateWindowElementList& FSlateDrawBuffer::add_window_element_list(std::shared_ptr<SWindow> for_window)
	{
		std::shared_ptr<FSlateWindowElementList> window_elements = std::make_shared<FSlateWindowElementList>(for_window);
		//todo:implement FSlateWindowElementList's ResetElementList
		m_window_element_lists.push_back(window_elements);

		return *window_elements;
	}
	void FSlateDrawBuffer::remove_unused_window_element(const std::vector<SWindow*>& all_windows)
	{
	}
}