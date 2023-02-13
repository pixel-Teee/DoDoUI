#include <PreCompileHeader.h>

#include "SlateStyleRegistry.h"

#include "SlateCore/Styling/ISlateStyle.h"

#include "Application/Application.h"

#include "SlateCore/Rendering/SlateRenderer.h"

#include "SlateCore/Styling/SlateBrush.h"

#include "Renderer/Renderer.h"

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
		if (Application::is_initialized())
		{
			Renderer* renderer = Application::get().get_renderer();
			if (renderer != nullptr)
			{
				//load style resources
				renderer->load_style_resources(in_slate_style);
			}
		}
	}
	void FSlateStyleRegistry::get_all_resources(std::vector<const FSlateBrush*>& out_resources)
	{
		//iterate the style chunks and collect their resources
		for (auto it = m_slate_style_repositry.begin(); it != m_slate_style_repositry.end(); ++it)
		{
			it->second->get_resources(out_resources);//from registry central's ISlateStyle to get the slate brush array
		}
	}
	const ISlateStyle* FSlateStyleRegistry::find_slate_style(const DoDoUtf8String& in_slate_style_name)
	{
		auto it = m_slate_style_repositry.find(in_slate_style_name);

		if (it == m_slate_style_repositry.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}
}