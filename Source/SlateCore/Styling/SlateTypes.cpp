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

	//const FSplitterStyle& FSplitterStyle::get_default()
	//{
	//	static FSplitterStyle default;
	//	return default;
	//}
}
