#pragma once

#include "Slate/Widgets/Layout/SBorder.h"//SBoder depends on it

namespace DoDo {
	/*
	* the header that apperas above lists and trees when they are showing multiple columns
	*/
	class SHeaderRow : public SBorder
	{
	public:
		/*describes a single column header*/
		class FColumn
		{
		public:
		};
	};
}