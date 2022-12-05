#include <PreCompileHeader.h>

#ifndef Android
	#include "Platform/Window/WindowsWindow.h"
#else
	#include "Platform/Window/AndroidWindow.h"
#endif

namespace DoDo {

	Scope<Window> Window::Create()
	{
#ifndef Android
		return CreateScope<WindowsWindow>();
#else
		return CreateScope<AndroidWindow>();
#endif
	}
}