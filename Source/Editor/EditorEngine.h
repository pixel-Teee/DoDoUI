#pragma once

namespace DoDo {
	class SWindow;
	class FSceneViewport;
	//struct FSlatePlayInEditorInfo {
	//public:
	//	/*the current play in editor SWindow if playing in a floating window*/
	//	std::weak_ptr<SWindow> m_slate_play_in_editor_window;
	//
	//	/*the current play in editor rendering and I/O viewport if playing in a floating window*/
	//	std::shared_ptr<FSceneViewport> m_slate_play_in_editor_window_viewport;
	//
	//	FSlatePlayInEditorInfo() {}
	//};

	class FEditorViewportClient;
	//editor
	class EditorEngine
	{
	public:
		EditorEngine();
		/*
		* updates a single viewport
		* @param Viewport - the viewport that we're trying to draw
		* @return - whether a non-relatime viewport has updated in this call, used to help time-slice canvas redraws
		*/
		bool update_single_viewport_client(FEditorViewportClient* in_viewport_client);

		/*
		* add a viewport client
		* @return index to the new item
		*/
		int32_t add_viewport_clients(FEditorViewportClient* viewport_client);
	public:
		virtual void init();

		virtual void tick(float delta_seconds, bool b_idle_mode);

	private:
		/*list of all viewport clients*/
		std::vector<FEditorViewportClient*> m_all_viewport_clients;
	};
}