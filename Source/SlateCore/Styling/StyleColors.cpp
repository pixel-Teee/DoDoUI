#include <PreCompileHeader.h>

#include "StyleColors.h"

namespace DoDo {
	USlateThemeManager USlateThemeManager::m_singleton;

	USlateThemeManager::USlateThemeManager()
	{

	}//note:default construct

	const FSlateColor FStyleColors::Foreground =		EStyleColor::Foreground;//use table id
	const FSlateColor FStyleColors::ForegroundHover =	EStyleColor::ForegroundHover;
	const FSlateColor FStyleColors::Panel =				EStyleColor::Panel;
	const FSlateColor FStyleColors::Primary =			EStyleColor::Primary;;
	const FSlateColor FStyleColors::Input =				EStyleColor::Input;
	const FSlateColor FStyleColors::Recessed =			EStyleColor::Recessed;
	const FSlateColor FStyleColors::PrimaryHover =		EStyleColor::PrimaryHover;
	const FSlateColor FStyleColors::PrimaryPress =		EStyleColor::PrimaryPress;
	const FSlateColor FStyleColors::DropDown =			EStyleColor::DropDown;
	const FSlateColor FStyleColors::Background =		EStyleColor::Background;
	const FSlateColor FStyleColors::Secondary =			EStyleColor::Secondary;
	const FSlateColor FStyleColors::Hover =				EStyleColor::Hover;

}