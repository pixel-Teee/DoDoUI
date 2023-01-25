#include <PreCompileHeader.h>

#include "GenericApplication.h"

#include "Platform/Application/GLFWApplication.h"

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
	std::shared_ptr<GenericApplication> GenericApplication::Create()
	{
		//return std::shared_ptr<GenericApplication>();
		//todo:implement android application
		return std::make_shared<GLFWApplication>();
	}
}