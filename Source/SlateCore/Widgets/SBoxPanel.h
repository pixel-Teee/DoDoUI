#pragma once

#include "SlateCore/Widgets/SPanel.h"

namespace DoDo {
	/*
	* a box panel contains one child and describes how that child should be arranged on the screen
	*/
	class SBoxPanel : public SPanel
	{
		SLATE_DECLARE_WIDGET(SBoxPanel, SPanel)

	protected:
		/*
		* a box panel contains one box panel child and describes how that
		* child should be arranged on the screen
		*/
		
	};
}