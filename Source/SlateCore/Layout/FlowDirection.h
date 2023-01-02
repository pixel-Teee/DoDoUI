#pragma once

namespace DoDo
{
	/*
	 * widgets may need to flow left or right depending upon the current culture/localization that's active
	 * this enum is used to request a specific layout flow
	 */
	enum class EFlowDirection : uint8_t
	{
		/*desires content flows using a LTR layout*/
		LeftToRight,

		/*desires content flows using a RTL layout*/
		RightToLeft
	};


	extern EFlowDirection g_flow_direction;
}
