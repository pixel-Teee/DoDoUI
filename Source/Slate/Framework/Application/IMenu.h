#pragma once

#include "Core/Core.h"

namespace DoDo {
	class SWindow;
	class SWidget;
	/*
	* represents a popup menu
	*/
	class IMenu
	{
	public:
		virtual ~IMenu() {}
		virtual std::shared_ptr<SWindow> get_parent_window() const = 0;
		virtual std::shared_ptr<SWindow> get_owned_window() const = 0;
		virtual std::shared_ptr<SWidget> get_content() const = 0;
	};
}