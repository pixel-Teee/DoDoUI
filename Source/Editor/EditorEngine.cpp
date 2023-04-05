#include "PreCompileHeader.h"

#include "EditorEngine.h"

#include "EditorViewportClient.h"//FEditorViewportClient depends on it

#include "Engine/Slate/SceneViewport.h"//FSceneViewport

namespace DoDo {
	EditorEngine* GEditor = nullptr;

	EditorEngine::EditorEngine()
	{

	}

	bool EditorEngine::update_single_viewport_client(FEditorViewportClient* in_viewport_client)
	{
		in_viewport_client->m_viewport->Draw();
		return true;
	}

	int32_t EditorEngine::add_viewport_clients(FEditorViewportClient* viewport_client)
	{
		m_all_viewport_clients.push_back(viewport_client);//note:this life time of viewport client is owned by the SEditorViewport
		
		return m_all_viewport_clients.size() - 1;
	}
	void EditorEngine::init()
	{
		GEditor = this;
	}
	void EditorEngine::tick(float delta_seconds, bool b_idle_mode)
	{
		for (int32_t viewport_index = m_all_viewport_clients.size() - 1; viewport_index >= 0; --viewport_index)
		{
			FEditorViewportClient* viewport_client = m_all_viewport_clients[viewport_index];

			//todo:add tick
			//viewport_client->tick(delta_seconds);
		}

		for (int32_t viewport_index = m_all_viewport_clients.size() - 1; viewport_index >= 0; --viewport_index)
		{
			FEditorViewportClient* viewport_client = m_all_viewport_clients[viewport_index];

			//todo:implement update single viewport client
			//viewport_client->tick(delta_seconds);
			//viewport_client->m_viewport->Draw();

			update_single_viewport_client(viewport_client);
		}
	}
}