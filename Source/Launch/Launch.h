#pragma once

namespace DoDo {
	class FEngineLoop
	{
	public:
		static void pre_init();

		static void loop();

		static void destroy();
	};
}