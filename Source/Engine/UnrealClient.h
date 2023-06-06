#pragma once

namespace DoDo {
	/*
	* an abstract interface to a viewport's client
	* the viewport's client processes input received by the viewport, and draws the viewport
	*/
	class FSceneViewport;
	class FViewportClient
	{
	public:
		virtual ~FViewportClient() {}

		virtual void Draw(FSceneViewport* viewport) {}
	};

	/*
	* common functionality for game and editor viewport clients
	*/
	class FCommonViewportClient : public FViewportClient
	{
	public:
		virtual ~FCommonViewportClient() {}
	};
}