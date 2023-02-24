#include <PreCompileHeader.h>

#include "Renderer.h"

//#include "Platform/VulkanRenderer/VulkanRenderer.h"

#include "Platform/VulkanRenderer/SlateVulkanRenderer.h"
#include "SlateCore/Fonts/FontMeasure.h"

namespace DoDo {
	FSlateFontServices::FSlateFontServices(std::shared_ptr<FSlateFontCache> in_game_thread_font_cache, std::shared_ptr<FSlateFontCache> in_render_thread_font_cache)
		: m_game_thread_font_cache(in_game_thread_font_cache)
		, m_render_thread_font_cache(in_render_thread_font_cache)
		, m_game_thread_font_measure(FSlateFontMeasure::Create(m_game_thread_font_cache))
	{
	}
	FSlateFontServices::~FSlateFontServices()
	{
	}

	std::shared_ptr<FSlateFontCache> FSlateFontServices::get_font_cache() const
	{
		return m_game_thread_font_cache;//note:just one thread
	}

	std::shared_ptr<FSlateFontMeasure> FSlateFontServices::get_font_measure_service() const
	{
		return  m_game_thread_font_measure;
	}

	Renderer::Renderer(const std::shared_ptr<FSlateFontServices>& in_slate_font_services)
		: m_slate_font_services(in_slate_font_services)
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::destroy()
	{
	}

	std::shared_ptr<Renderer> Renderer::Create()
	{
		return std::make_shared<FSlateVulkanRenderer>();
	}
	
}
