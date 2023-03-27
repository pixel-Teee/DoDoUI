#pragma once

#include "SlateCore/Widgets/SPanel.h"//SPanel

namespace DoDo {
	struct FSlateBrush;
	class SNodePanel : public SPanel
	{
	public:

		SNodePanel();

		//SPanel interface
		
		//End of SPanel interface

		//SWidget interface

		//End of SWidget interface

	protected:
		//paint the background as lines
		void paint_background_as_lines(const FSlateBrush* background_image, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t& draw_layer_id) const;
	};
}