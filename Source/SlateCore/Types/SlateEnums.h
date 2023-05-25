#pragma once

namespace DoDo
{
	/*
	* generic ui navigation action that are usually bound to a game pad or other input device
	*/
	enum class EUINavigationAction : uint8_t
	{
		/*accept, normally acts like a mouse click, corresponds to virtual_accept key*/
		Accept,

		/*back/cancel, normally acts like escape, corresponds to virtual_back key*/
		Back,

		/*number of navigation actions*/
		Num,

		/*invalid action*/
		Invalid
	};

	/*
	* navigation context for event
	*/
	enum class EUINavigation : uint8_t
	{
		/*four cardinal directions*/
		Left,
		Right,
		Up,
		Down,

		/*conceptual next and previous*/
		Next,
		Previous,

		/*number of navigation types*/
		Num,

		/*denotes an invalid operation*/
		Invalid
	};
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
