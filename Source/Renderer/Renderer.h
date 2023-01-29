#pragma once

#include "Core/Core.h"

namespace DoDo
{
	class SWindow;
	//TODO:temporarily use this renderer
	class Renderer
	{
	public:
		//UIRenderer();

		/*
		 * creates a rendering viewport
		 *
		 * @param InWindow the window to create the viewport for
		 */
		virtual void create_view_port(const std::shared_ptr<SWindow> in_window) = 0;

		virtual bool initialize() = 0;

		virtual ~Renderer();

		virtual void destroy();

		static std::shared_ptr<Renderer> Create();//transfer owner ship
	};
}