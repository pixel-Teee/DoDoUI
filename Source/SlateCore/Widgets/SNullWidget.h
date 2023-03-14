#pragma once

#include <memory>

namespace DoDo {
	class SWidget;
	/*
	* implements a widget that can be used as a placeholder
	* 
	* widgets that supports slots, such as SOverlay and SHorizontalBox should initialize
	* their slot's child widgets to SNullWidget if no user defined widget was provided
	*/
	class SNullWidget
	{
	public:
		/*
		* returns a placeholder widget
		* 
		* @return the widget
		*/
		static std::shared_ptr<SWidget> NullWidget;
	};
}