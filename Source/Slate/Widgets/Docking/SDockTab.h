#pragma once

#include "Slate/Widgets/Layout/SBorder.h"

namespace DoDo {
	class FTabManager;
	class SDockTab : public SBorder
	{

	protected:
		/*the tab manager that created this tab*/
		std::weak_ptr<FTabManager> m_my_tab_manager;
	};
}