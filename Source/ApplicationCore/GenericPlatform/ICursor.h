#pragma once

#include "glm/glm.hpp"

namespace DoDo
{
	/*
	 * mouse cursor types
	 */
	namespace EMouseCursor
	{
		enum Type
		{
			/*causes no mouse cursor to be visible*/
			None,

			/*default cursor (arrow)*/
			Default,

			/*text edit beam*/
			TextEditBeam,

			/*resize horizontal*/
			ResizeLeftRight,

			/*resize vertical*/
			ResizeUpDown,

			/*resize diagonal*/
			ResizeSouthEast,

			/*Resize other diagonal*/
			ResizeSouthWest,

			/*move item*/
			CardinalCross,

			/*target cross*/
			Crosshairs,

			/*hand cursor*/
			Hand,

			/*grab hand cursor*/
			GrabHand,

			/*grab hand cursor closed*/
			GrabHandClosed,

			/*a circle with a diagonal line through it*/
			SlashedCircle,

			/*eve-dropper cursor for picking colors*/
			EyeDropper,

			/*custom cursor shape for platforms that support setting a native cursor shape, same as specifying none if not set*/
			Custom,

			/*number of cursors we support*/
			TotalCursorCount
		};
	}

	class Window;
	class ICursor
	{
	public:

		virtual glm::vec2 get_position() const = 0;

		virtual void set_position(const int32_t x, const int32_t y) = 0;

		virtual void set_type(const EMouseCursor::Type in_new_cursor) = 0;

		virtual void set_type(const std::shared_ptr<Window> window, const EMouseCursor::Type in_new_cursor) = 0;
	};
}