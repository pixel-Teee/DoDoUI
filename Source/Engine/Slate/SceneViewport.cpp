#include "PreCompileHeader.h"

#include "SceneViewport.h"

#include "Editor/EditorViewportClient.h"//FViewportClient depends on it

namespace DoDo {
	FSceneViewport::FSceneViewport(FViewportClient* in_viewport_client, std::shared_ptr<SViewport> in_viewport_widget)
		: m_viewport_client(in_viewport_client)
		, m_viewport_widget(in_viewport_widget)
		, m_b_requires_vsync(false)
	{
	}
	FSceneViewport::~FSceneViewport()
	{
	}
	FSlateShaderResource* FSceneViewport::get_viewport_render_target_texture() const
	{
		return nullptr;
	}
	void FSceneViewport::Draw(bool b_should_present)
	{
		m_viewport_client->Draw(this);
	}
}