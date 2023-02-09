#pragma once

#include "SWidget.h"

namespace DoDo
{
	class SLeafWidget : public SWidget
	{
	public:
		SLeafWidget()
		{
			b_can_have_children = false;
		}

		//todo:implement set visibility

	private:
		//todo:implement these functions
	};
}
