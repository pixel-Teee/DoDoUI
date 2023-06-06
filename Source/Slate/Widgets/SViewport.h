#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"//SCompoundWidget

namespace DoDo {
	class ISlateViewport;//viewport

	class SViewport : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SViewport)
			: _Content()
		{
			//todo:set clipping
		}
			SLATE_DEFAULT_SLOT(FArguments, Content)

			/*whether or not to show the disabled effect when this viewport is disabled*/
			SLATE_ATTRIBUTE(bool, ShowEffectWhenDisabled)

			/*size of the viewport widget*/
			SLATE_ATTRIBUTE(glm::vec2, ViewportSize)

			
		SLATE_END_ARGS()

		SViewport();

		void Construct(const FArguments& InArgs);

	protected:
		/*interface to the rendering and I/O implementation of the viewport*/
		std::weak_ptr<ISlateViewport> m_viewport_interface;

	private:
		/*the parent window during this viewport's last activation*/
		std::weak_ptr<SWindow> m_cached_parent_window;
	};
}