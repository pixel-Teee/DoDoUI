#pragma once

#include "Core/Containers/EnumAsBytes.h"

namespace DoDo
{
	//is an entity visible?
	struct EVisibility
	{
		/*visible and hit-testable(can interact with cursor), default value*/
		static const EVisibility visible;

		/*
		 * default constructor
		 * the default visibility is 'visible'
		 */
		EVisibility()
			: m_Value(VIS_Visible)
		{
			
		}

	private:
		enum Private
		{
			//entity is visible
			VISPRIVATE_Visiable = 0x1 << 0,

			//entity is invisible and takes up no space
			VISPRIVATE_Collapsed = 0x1 << 1,

			//entity is invisible, but still takes up space(layout pretends it is visible)
			VISPRIVATE_Hidden = 0x1 << 2,

			//can we click on this widget or is it just non-interactive decoration?
			VISPRIVATE_SelfHitTestVisible = 0x1 << 3,

			//can we click on this widget's child widgets?
			VISPRIVATE_ChildrenHitTestVisible = 0x1 << 4,

			//default widget visibility - visible and can interactive with the cursor
			VIS_Visible = VISPRIVATE_Visiable | VISPRIVATE_SelfHitTestVisible | VISPRIVATE_ChildrenHitTestVisible,

			VIS_Collapsed = VISPRIVATE_Collapsed,

			VIS_Hidden = VISPRIVATE_Hidden,

			VIS_HitTestInvisible = VISPRIVATE_Visiable,

			VIS_SelfHitTestInvisible = VISPRIVATE_Visiable | VISPRIVATE_ChildrenHitTestVisible,

			VIS_All = VISPRIVATE_Visiable | VISPRIVATE_Hidden | VISPRIVATE_Collapsed | VISPRIVATE_SelfHitTestVisible | VISPRIVATE_ChildrenHitTestVisible
		};
	public:
		TEnumAsByte<Private> m_Value;
	};
}
