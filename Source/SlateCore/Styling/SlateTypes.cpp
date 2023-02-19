#include <PreCompileHeader.h>

#include "SlateTypes.h"

namespace DoDo
{
	const DoDoUtf8String FButtonStyle::TypeName("FButtonStyle");
	FButtonStyle::FButtonStyle()
	{
	}

	FButtonStyle::~FButtonStyle()
	{
	}

	const DoDoUtf8String FSplitterStyle::TypeName("FSplitterStyle");
	FSplitterStyle::FSplitterStyle()
	{
	}

	FSplitterStyle::~FSplitterStyle()
	{
	}

	void FSplitterStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		//FSlateWidgetStyle::get_resources(out_brushes);
		out_brushes.push_back(&m_handled_normal_brush);
		out_brushes.push_back(&m_handle_highlight_brush);
	}

	FDockTabStyle::FDockTabStyle()
		: m_icon_size(16, 16)
		, m_overlap_width(0.0f)
		, m_normal_foreground_color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
		, m_hovered_foreground_color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
		, m_active_foreground_color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
		, m_foreground_foreground_color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
		, m_icon_border_padding(1.0f)
	{
	}

	FDockTabStyle::~FDockTabStyle()
	{
	}

	void FDockTabStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
		m_close_button_style.get_resources(out_brushes);

		out_brushes.push_back(&m_normal_brush);
		out_brushes.push_back(&m_color_overlay_tab_brush);
		out_brushes.push_back(&m_color_overlay_icon_brush);
		out_brushes.push_back(&m_foreground_brush);
		out_brushes.push_back(&m_hovered_brush);
		out_brushes.push_back(&m_content_area_brush);
		out_brushes.push_back(&m_tab_well_brush);
	}

	const DoDoUtf8String FDockTabStyle::TypeName("FDockTabStyle");

	const FDockTabStyle& FDockTabStyle::get_default()
	{
		static FDockTabStyle Default;
		return Default;
	}

	FTextBlockStyle::FTextBlockStyle()
	{
	}

	void FTextBlockStyle::get_resources(std::vector<const FSlateBrush*>& out_brushes) const
	{
	}

	const DoDoUtf8String FTextBlockStyle::TypeName("FTextBlockStyle");

	//const FSplitterStyle& FSplitterStyle::get_default()
	//{
	//	static FSplitterStyle default;
	//	return default;
	//}
}
