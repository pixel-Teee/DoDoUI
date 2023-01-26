#include <PreCompileHeader.h>

#ifndef Android
	#include "Platform/Window/WindowsWindow.h"
#else
	#include "Platform/Window/AndroidWindow.h"
#endif

namespace DoDo {
	void Window::Update(RendererInstance& render_instance)
	{
	}
	void* Window::get_window_native_handle()
	{
		return nullptr;
	}
	Scope<Window> Window::Create()
	{
#ifndef Android
		return CreateScope<WindowsWindow>();
#else
		return CreateScope<AndroidWindow>();
#endif
	}
}