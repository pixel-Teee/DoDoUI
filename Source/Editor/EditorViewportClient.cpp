#include "PreCompileHeader.h"

#include "EditorViewportClient.h"

#include "EditorEngine.h"//EditorEngine depends on it

namespace DoDo {
	extern class EditorEngine* GEditor;
	FEditorViewportClient::FEditorViewportClient(const std::weak_ptr<SEditorViewport>& in_editor_viewport_widget)
		: m_editor_viewport_widget(in_editor_viewport_widget)
	{
		//todo:add this client to list of views, and remember the index
		GEditor->add_viewport_clients(this);
	}
	FEditorViewportClient::~FEditorViewportClient()
	{
	}
	void FEditorViewportClient::Draw(FSceneViewport* in_viewport)
	{
		//todo:get renderer and draw viewport
		
		//from in_viewport to get render target and draw

	}
}
