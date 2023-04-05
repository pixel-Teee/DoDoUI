#include "PreCompileHeader.h"

#include "SEditorViewport.h"

#include "EditorViewportClient.h"

#include "Engine/Slate/SceneViewport.h"//FSceneViewport depends on it

namespace DoDo {
	SEditorViewport::SEditorViewport()
	{
	}
	SEditorViewport::~SEditorViewport()
	{
	}
	void SEditorViewport::Construct(const FArguments& in_args)
	{
		m_child_slot
		[
			SAssignNew(m_viewport_widget, SViewport)
			.ViewportSize(in_args._ViewportSize)
		];
		
		m_client = make_editor_viewport_client();

		m_scene_viewport = std::make_shared<FSceneViewport>(m_client.get(), m_viewport_widget);//todo:implement viewport widget

		m_client->m_viewport = m_scene_viewport.get();//client and FSceneViewport holds the raw pointer each other

		m_viewport_widget->set_viewport_interface(m_scene_viewport);//SViewport will from FSceneViewport to get the render target handle
	}
	std::shared_ptr<FEditorViewportClient> SEditorViewport::make_editor_viewport_client()
	{
		if (!m_client)
		{
			m_client = std::make_shared<FEditorViewportClient>(std::static_pointer_cast<SEditorViewport>(shared_from_this()));//hold the client life time
		}

		return m_client;
	}
}