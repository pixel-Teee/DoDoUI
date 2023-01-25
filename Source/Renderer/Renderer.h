#pragma once

#include "Core/Core.h"

namespace DoDo
{
	//TODO:temporarily use this renderer
	class Renderer
	{
	public:
		//UIRenderer();

		virtual ~Renderer();

		static std::shared_ptr<Renderer> Create();//transfer owner ship
	};
}