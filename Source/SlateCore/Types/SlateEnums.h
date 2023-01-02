#pragma once

namespace DoDo
{
	/*
	 * enumerates horizontal alignment options, i.e. for widget slots
	 */
	enum EHorizontalAlignment
	{
		/*fill the entire width*/
		HAlign_Fill,

		/*left align*/
		HAlign_Left,

		/*center align*/
		HAlign_Center,

		/*right align*/
		HAlign_Right
	};

	/*
	 * enumerates vertical alignment options, i.e. for widget slots
	 */
	enum EVerticalAlignment
	{
		/*fill the entire height*/
		VAlign_Fill,

		/*top-align*/
		VAlign_Top,

		/*center-align*/
		VAlign_Center,

		/*bottom-align*/
		VAlign_Bottom
	};

	/*
	 * enumerates widget orientations
	 */
	enum EOrientation
	{
		/*orient horizontally, left to right*/
		Orient_Horizontal,

		/*orient vertically, top to bottom*/
		Orient_Vertical
	};
}
