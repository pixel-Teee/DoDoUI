#pragma once

#include "Core/Containers/EnumAsBytes.h"

namespace DoDo
{
	//is an entity visible?
	struct EVisibility
	{
		/*not visible and takes up no space in the layout(obviously not hit-testable)*/
		static const EVisibility Collapsed;

		/*visible and hit-testable(can interact with cursor), default value*/
		static const EVisibility visible;


		/*any visibility will do*/
		static const EVisibility All;

		/*
		 * default constructor
		 * the default visibility is 'visible'
		 */
		EVisibility()
			: m_Value(VIS_Visible)
		{
			
		}

	public:
		bool operator==(const EVisibility& other) const
		{
			return this->m_Value == other.m_Value;
		}

		bool operator!=(const EVisibility& other) const
		{
			//implicit conversion and compare
			return this->m_Value != other.m_Value;
		}

		//query the filter have the in_visibility, if don't have, return false, else return true
		static bool Does_Visibility_Pass_Filter(const EVisibility in_visibility, const EVisibility in_visibility_filter)
		{
			return 0 != (in_visibility.m_Value & in_visibility_filter.m_Value);
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

		/*
		 * private constructor
		 */
		EVisibility(Private in_value)
			: m_Value(in_value)
		{
			
		}
	public:
		TEnumAsByte<Private> m_Value;
	};
}
