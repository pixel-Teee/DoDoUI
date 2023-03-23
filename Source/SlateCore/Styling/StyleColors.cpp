#include <PreCompileHeader.h>

#include "StyleColors.h"

namespace DoDo {
	USlateThemeManager USlateThemeManager::m_singleton;

	USlateThemeManager::USlateThemeManager()
	{
	}//note:default construct

	const FSlateColor FStyleColors::Foreground = EStyleColor::Foreground;//use table id
	const FSlateColor FStyleColors::ForegroundHover = EStyleColor::ForegroundHover;
	const FSlateColor FStyleColors::Panel = EStyleColor::Panel;

}