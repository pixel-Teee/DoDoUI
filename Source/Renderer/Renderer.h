#pragma once

#include "Core/Core.h"

namespace DoDo
{
	class FSlateDrawBuffer;
	class SWindow;
	class ISlateStyle;
	//TODO:temporarily use this renderer
	class Renderer
	{
	public:
		//UIRenderer();

		/*returns a draw buffer that can be used by slate windows to draw window elements*/
		virtual FSlateDrawBuffer& get_draw_buffer() = 0;

		/*
		 * creates a rendering viewport
		 *
		 * @param InWindow the window to create the viewport for
		 */
		virtual void create_view_port(const std::shared_ptr<SWindow> in_window) = 0;

		/*
		* loads all the resources used by the specified slate style
		*/
		virtual void load_style_resources(const ISlateStyle& style) {}

		/*
		 * creates necessary resources to render a window and sends draw commands to the rendering thread?
		 *
		 * @param WindowDrawBuffer the buffer containing elements to draw
		 */
		virtual void draw_windows(FSlateDrawBuffer& in_window_draw_buffer) = 0;

		virtual bool initialize() = 0;

		virtual ~Renderer();

		virtual void destroy();

		static std::shared_ptr<Renderer> Create();//transfer owner ship
	};
}