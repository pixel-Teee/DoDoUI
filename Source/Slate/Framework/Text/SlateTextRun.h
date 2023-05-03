#pragma once

#include "ISlateRun.h"//ISlateRun depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

namespace DoDo {
	class FSlateTextRun : public ISlateRun, public std::enable_shared_from_this<FSlateTextRun>
	{
	public:
		virtual ~FSlateTextRun();

		virtual int32_t On_Paint(const FPaintArgs& paint_args, const FTextArgs& text_args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect,
			FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
	protected:

		FRunInfo m_run_info;

		std::shared_ptr<const DoDoUtf8String> m_text;
		FTextBlockStyle m_style;
		FTextRange m_range;
	};
}