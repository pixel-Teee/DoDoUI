#pragma once

#include "Core/Core.h"

namespace DoDo
{
	//TODO:temporarily use this renderer
	class UIRenderer
	{
	public:
		//UIRenderer();

		virtual ~UIRenderer();	

		static Scope<UIRenderer> Create();//transfer owner ship
	};
}