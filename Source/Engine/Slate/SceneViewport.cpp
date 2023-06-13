#include "PreCompileHeader.h"

#include "SceneViewport.h"

#include "Editor/EditorViewportClient.h"//FViewportClient depends on it

#include "Engine/Slate/SlateTextures.h"//FSlateRenderTargetRHI depeneds on it

namespace DoDo {
	FSceneViewport::FSceneViewport(FViewportClient* in_viewport_client, std::shared_ptr<SViewport> in_viewport_widget)
		: m_viewport_client(in_viewport_client)
		, m_viewport_widget(in_viewport_widget)
		, m_b_requires_vsync(false)
	{
		m_slate_texture = new FSlateRenderTargetRHI(nullptr, 0, 0);

		//todo:bind function

		//todo:move these logic to init dynamic rhi

		//create texture

	}
	FSceneViewport::~FSceneViewport()
	{
	}
	FSlateShaderResource* FSceneViewport::get_viewport_render_target_texture() const
	{
		return nullptr;
	}
	void FSceneViewport::Draw(bool b_should_present) //todo:move this function to FViewport
	{
		m_viewport_client->Draw(this);
	}
}