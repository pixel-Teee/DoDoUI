#include <PreCompileHeader.h>

#include "Platform/Window/WindowsWindow.h"

namespace DoDo {
	Scope<Window> Window::Create()
	{
		return CreateScope<WindowsWindow>();
	}
}