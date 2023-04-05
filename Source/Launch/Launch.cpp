#include "PreCompileHeader.h"

#include "Launch.h"

#include <Application/Application.h>

#include <Slate/Framework/Docking/TabManager.h>

#include <Renderer/Renderer.h>//renderer

#include "Core/String/DoDoString.h"

#include <fstream>

#include "Core/Delegates/DelegateCombinations.h"

#include "Core/Misc/CoreGlobals.h"

#include "Editor/EditorEngine.h"

#ifdef WIN32
#include "Platform/Application/WindowsPlatformApplicationMisc.h"
#endif

namespace DoDo {
	extern class EditorEngine* GEditor;
	void FEngineLoop::pre_init()
	{
		DoDo::FPlatformApplicationMisc::platform_pre_init();//todo:need to put at the app init function

		//delete GEditor;	
		GEditor = new EditorEngine();

		GEditor->init();

		DoDo::Application::Create();//initialize platform application

		std::shared_ptr<DoDo::Renderer> pRenderer = DoDo::Renderer::Create();

		DoDo::Application::get().Initialize_Renderer(pRenderer);//initialize renderer

		DoDo::FGlobalTabmanager::get()->set_application_title("my app");

		DoDo::Application::get().set_exit_requested_handler(DoDo::FSimpleDelegate::CreateStatic(&DoDo::request_engine_exit));

		DoDo::Application::get().test_create_widget();
	}
	void FEngineLoop::loop()
	{
		//tick
		//todo:add exit global variable
		while (!DoDo::is_engine_exit_requested())
		{
			DoDo::Application::get().Tick();
		}
	}
	void FEngineLoop::destroy()
	{
		DoDo::Application::shut_down();//todo:in the future, put this at exit function

		delete GEditor;
	}
}