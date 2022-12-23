#pragma once

namespace DoDo
{
	//is an entity visible?
	struct EVisibility
	{
		/*visible and hit-testable(can interact with cursor), default value*/
		static const EVisibility visible;


	private:
		enum Private
		{
			//entity is visible
			VISPRIVATE_Visable = 0x1 << 0,

			//entity is invisible and takes up no space
			VISPRIVATE_Collapsed = 0x1 << 1,

			//entity is invisible, but still takes up space(layout pretends it is visible)
			VISPRIVATE_Hidden = 0x1 << 2,

			//can we click on this widget or is it just non-interactive decoration?
			VISPRIVATE_SelfHitTestVisible = 0x1 << 3
		};

	};
}
