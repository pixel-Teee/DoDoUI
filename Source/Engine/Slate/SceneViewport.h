#pragma once

#include "SlateCore/Rendering/RenderingCommon.h"//ISlateViewport depends on it

namespace DoDo {
	class SViewport;
	class FViewportClient;
	/*
	* a viewport for use with slate SViewport widgets
	*/

	class FSceneViewport : public ISlateViewport
	{
	public:
		FSceneViewport(FViewportClient* in_viewport_client, std::shared_ptr<SViewport> in_viewport_widget);//todo:need add FViewportClient

		~FSceneViewport();

		virtual class FSlateShaderResource* get_viewport_render_target_texture() const;

		/*
		* updates the viewport's displayed pixels with the results of calling viewport client->draw
		* 
		* @param bShouldPresent whether we want this frame to be presented
		*/
		void Draw(bool b_should_present = true);

		/*
		* returns true if the viewport should be vsynced
		*/
		virtual bool requires_vsync() const override { return m_b_requires_vsync; }

	protected:
		/*the viewport's client*/
		FViewportClient* m_viewport_client;

	private:

		/*the slate viewport widget where this viewport is drawn*/
		std::weak_ptr<SViewport> m_viewport_widget;

		/*true if this viewport requires vsync*/
		bool m_b_requires_vsync;

		/*the render target used by slate to draw the viewport, can be null if this viewport renders directly to the backbuffer*/
		std::vector<class FSlateRenderTargetRHI*> m_buffered_slate_handles;//todo:fix me
	};
}