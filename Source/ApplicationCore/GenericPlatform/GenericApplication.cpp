#include <PreCompileHeader.h>

#include "GenericApplication.h"

#ifdef WIN32
#include "Platform/Application/GLFWApplication.h"
#else
#include "Platform/Application/AndroidApplication.h"
#endif

#include "Core/Window.h"

namespace DoDo {
	GenericApplication::GenericApplication(const std::shared_ptr<ICursor>& in_cursor)
		: m_cursor(in_cursor)
	{
	}
	GenericApplication::~GenericApplication()
	{
	}
	void GenericApplication::Tick(const float time_delta)
	{
	}
	std::shared_ptr<Window> GenericApplication::make_window()
	{
		return std::make_shared<Window>();//todo:implement platform application's make window
	}
	std::shared_ptr<GenericApplication> GenericApplication::Create()
	{
		//return std::shared_ptr<GenericApplication>();
		//todo:implement android application
#ifdef WIN32
		return std::make_shared<GLFWApplication>();
#else
		return std::make_shared<AndroidApplication>();
#endif
	}
}