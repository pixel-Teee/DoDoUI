#pragma once

namespace DoDo {
	struct FSlateBrush;
	class FStyleDefaults
	{
	public:
		/*
		* @return - returns no brush
		*/
		static const FSlateBrush* get_no_brush();
	};
}