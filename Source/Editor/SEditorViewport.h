#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "Slate/Widgets/SViewport.h"//SViewport Depends on it

namespace DoDo {
	class SOverlay;
	class FSceneViewport;
	class SViewport;
	class FEditorViewportClient;
	//note:this life time of client viewport and FSceneViewport is owned by the SEditorViewport
	class SEditorViewport : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SEditorViewport)
			: _ViewportSize(SViewport::FArguments::get_default_viewport_size())
		{}
		SLATE_ATTRIBUTE(glm::vec2, ViewportSize)
		SLATE_END_ARGS()

		SEditorViewport();

		virtual ~SEditorViewport();

		void Construct(const FArguments& in_args);

		virtual std::shared_ptr<FEditorViewportClient> make_editor_viewport_client();

	protected:
		std::shared_ptr<SOverlay> m_viewport_overlay;

		/*viewport that renders the scene provided by the viewport client*/
		std::shared_ptr<FSceneViewport> m_scene_viewport;

		/*widget where the scene viewport is drawn in*/
		std::shared_ptr<SViewport> m_viewport_widget;

		/*the client responsible for setting up the scene*/
		std::shared_ptr<FEditorViewportClient> m_client;
	};
}