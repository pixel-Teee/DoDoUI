#pragma once

#include "Core/Core.h"

#include "SlateCore/Textures/SlateShaderResource.h"//FSlateResourceHandle depends on it

namespace DoDo
{
	struct FSlateBrush;
	class FSlateDrawBuffer;
	class SWindow;
	class ISlateStyle;
	class FSlateFontCache;
	/*
	* provides access to the game and render thread font caches that slate should use
	*/
	class FSlateFontServices
	{
	public:
		/*
		* construct the font services from the font cache
		* 
		* these pointers may be the same if your renderer doesn't need a separate render thread font cache
		*/
		FSlateFontServices(std::shared_ptr<FSlateFontCache> in_game_thread_font_cache, std::shared_ptr<FSlateFontCache> in_render_thread_font_cache);

		~FSlateFontServices();

		/*
		* get the font cache to use for the current thread
		*/
		std::shared_ptr<FSlateFontCache> get_font_cache() const;

	private:
		std::shared_ptr<FSlateFontCache> m_game_thread_font_cache;
		std::shared_ptr<FSlateFontCache> m_render_thread_font_cache;
	};

	//TODO:temporarily use this renderer
	class Renderer
	{
	public:
		//UIRenderer();
		Renderer(const std::shared_ptr<FSlateFontServices>& in_slate_font_services);

		/*returns a draw buffer that can be used by slate windows to draw window elements*/
		virtual FSlateDrawBuffer& get_draw_buffer() = 0;

		/*
		 * creates a rendering viewport
		 *
		 * @param InWindow the window to create the viewport for
		 */
		virtual void create_view_port(const std::shared_ptr<SWindow> in_window) = 0;

		/*
		* loads all the resources used by the specified slate style
		*/
		virtual void load_style_resources(const ISlateStyle& style) {}

		/*
		 * creates necessary resources to render a window and sends draw commands to the rendering thread?
		 *
		 * @param WindowDrawBuffer the buffer containing elements to draw
		 */
		virtual void draw_windows(FSlateDrawBuffer& in_window_draw_buffer) = 0;

		virtual FSlateResourceHandle get_resource_handle(const FSlateBrush& brush, glm::vec2 local_size, float draw_scale) = 0;//todo:add comment

		virtual bool initialize() = 0;

		virtual ~Renderer();

		virtual void destroy();

		static std::shared_ptr<Renderer> Create();//transfer owner ship

	protected:
		/*the font services used by this renderer when drawing text*/
		std::shared_ptr<FSlateFontServices> m_slate_font_services;
	};
}