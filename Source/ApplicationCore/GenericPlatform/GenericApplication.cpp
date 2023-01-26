#include <PreCompileHeader.h>

#include "GenericApplication.h"

#include "Platform/Application/GLFWApplication.h"

#include "Core/Window.h"

namespace DoDo {
	GenericApplication::GenericApplication()
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
		return std::make_shared<GLFWApplication>();
	}
}