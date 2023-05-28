#pragma once

#include "glm/glm.hpp"

#include <memory> //shared_ptr

namespace DoDo {
	class SWidget;
	/*
	* an interface to allow slate widgets to expose scrollable functionality
	*/
	class IScrollableWidget
	{
	public:
		virtual ~IScrollableWidget() {}

		/*gets the distance that user has scrolled into the control in normalized coordinates (0 - 1)*/
		virtual glm::vec2 get_scroll_distance() = 0;

		/*gets the distance that user has left to scroll in the control before reaching the end in normalized coordinates (0 - 1)*/
		virtual glm::vec2 get_scroll_distance_remaining() = 0;

		/*returns the typed SWidget that implements this interface*/
		virtual std::shared_ptr<SWidget> get_scroll_widget() = 0;
	};
}