#pragma once

#include "SWidget.h"

#include "Core/Misc/Attribute.h"

namespace DoDo {
	class SCompoundWidget : public SWidget
	{
	public:
		SCompoundWidget();

		virtual ~SCompoundWidget();

	private:

		//the layout scale to apply to this widget's contents, useful for animation

	};
}