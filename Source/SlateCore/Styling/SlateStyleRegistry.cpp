#include <PreCompileHeader.h>

#include "SlateStyleRegistry.h"

#include "SlateCore/Styling/ISlateStyle.h"

#include "Application/Application.h"

#include "SlateCore/Rendering/SlateRenderer.h"

namespace DoDo {
	std::map<DoDoUtf8String, const ISlateStyle*> FSlateStyleRegistry::m_slate_style_repositry;//default construct

	void FSlateStyleRegistry::register_slate_style(const ISlateStyle& in_slate_style)
	{
		//register
		const DoDoUtf8String& slate_style_name = in_slate_style.get_style_set_name();

		//todo:check

		m_slate_style_repositry.insert({ slate_style_name, &in_slate_style });

		//todo:check slate application initialized?
		//Renderer* renderer = Application::get().get_renderer();

		//todo:load style resources

	}
}