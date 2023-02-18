#pragma once

namespace DoDo
{
	class FSlateRenderBatch;
	class FSlateFontCache;
	class FSlateFontServices;
	/*abstract base class for rendering policies*/
	class FSlateRenderingPolicy
	{
	public:
		FSlateRenderingPolicy(std::shared_ptr<FSlateFontServices> in_slate_font_services);

		virtual ~FSlateRenderingPolicy() {}

		std::shared_ptr<FSlateFontCache> get_font_cache() const;
	private:

		std::shared_ptr<FSlateFontServices> m_font_services;//the life time of this font service is create from 
	};
}
