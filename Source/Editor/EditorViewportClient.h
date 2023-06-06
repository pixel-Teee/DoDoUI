#pragma once

#include "Core/Core.h"//weak_ptr

#include "Engine/UnrealClient.h"//FCommonViewport Client depends on it

namespace DoDo {
	class SEditorViewport;
	class FSceneViewport;//note:contains a render target
	/*viewport client for editor viewports, contains common functionality for camera movement, rendering debug information, etc.*/
	class FEditorViewportClient : public FCommonViewportClient
	{
	public:
		FEditorViewportClient(const std::weak_ptr<SEditorViewport>& in_editor_viewport_widget);

		virtual ~FEditorViewportClient();

		void Draw(FSceneViewport* view) override;

		FSceneViewport* m_viewport;
	private:

		/*the editor viewport widget this client is attached to*/
		std::weak_ptr<SEditorViewport> m_editor_viewport_widget;
	};
}