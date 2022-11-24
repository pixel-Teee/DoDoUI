#pragma once

namespace DoDo
{
	//TODO:temporarily use this renderer
	class UIRenderer
	{
	public:
		//UIRenderer();

		virtual ~UIRenderer();	

		static std::unique_ptr<UIRenderer> Create();//transfer owner ship
	};
}