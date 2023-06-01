#pragma once

namespace DoDo {
	class IBreakIterator;
	struct FBreakIterator {
		/*
		* create a new instance of a break iterator designed to locate character boundary points within a string
		*/
		static std::shared_ptr<IBreakIterator> create_character_boundary_iterator();
	};
}