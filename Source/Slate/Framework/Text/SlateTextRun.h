#pragma once

#include "ISlateRun.h"//ISlateRun depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

namespace DoDo {
	class FSlateTextRun : public ISlateRun, public std::enable_shared_from_this<FSlateTextRun>
	{
	public:
		virtual ~FSlateTextRun();

		static std::shared_ptr<FSlateTextRun> Create(const FRunInfo& in_run_info, const std::shared_ptr<DoDoUtf8String>& in_text, const FTextBlockStyle& style);

		virtual const std::vector<std::shared_ptr<SWidget>>& get_children() override;

		virtual int32_t On_Paint(const FPaintArgs& paint_args, const FTextArgs& text_args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect,
			FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		FSlateTextRun(const FRunInfo& in_run_info, const std::shared_ptr<const DoDoUtf8String>& in_text, const FTextBlockStyle& in_style);
	protected:

		FRunInfo m_run_info;

		std::shared_ptr<const DoDoUtf8String> m_text;
		FTextBlockStyle m_style;
		FTextRange m_range;
	};
}