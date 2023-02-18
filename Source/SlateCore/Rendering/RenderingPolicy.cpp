#include <PreCompileHeader.h>

#include "RenderingPolicy.h"

#include "SlateRenderBatch.h"//FSlateRenderBatch

#include "Renderer/Renderer.h"//FSlateFontServices

namespace DoDo
{
    FSlateRenderingPolicy::FSlateRenderingPolicy(std::shared_ptr<FSlateFontServices> in_slate_font_services)
        : m_font_services(in_slate_font_services)
    {
    }
    std::shared_ptr<FSlateFontCache> FSlateRenderingPolicy::get_font_cache() const
    {
        return m_font_services->get_font_cache();
    }
}
