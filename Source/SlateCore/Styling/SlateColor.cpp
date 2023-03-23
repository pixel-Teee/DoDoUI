#include "PreCompileHeader.h"
#include "SlateColor.h"

#include "StyleColors.h"//USlateThemeManager depends on it

namespace DoDo {
	const FLinearColor& DoDo::FSlateColor::get_color_from_table() const
	{
		//todo:from theme manager to get color
		return USlateThemeManager::get().get_color(m_color_table_id);
	}
}
