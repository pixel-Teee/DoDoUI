#include "PreCompileHeader.h"

#include "UMGCoreStyle.h"

#include "SlateCore/Styling/SlateStyle.h"//FSlateStyleSet depends on it

#include "Core/Misc/Paths.h"//FPaths depends on it

#include "SlateCore/Styling/SlateTypes.h"//FButtonStyle depends on it

#include "SlateCore/Styling/CoreStyle.h"

#include "SlateCore/Brushes/SlateRoundedBoxBrush.h"//FSlateRoundedBoxBrush depends on it

#include "SlateCore/Brushes/SlateImageBrush.h"//FSlateImageBrush depends on it

#include "SlateCore/Styling/SlateStyleRegistry.h"//FSlateStyleRegistry depends on it

namespace DoDo {
	using namespace CoreStyleConstants;//CoreStyle

	std::shared_ptr<ISlateStyle> FUMGCoreStyle::m_instance = nullptr;

	class FUMGStyleSet : public FSlateStyleSet
	{
	public:
		FUMGStyleSet(const DoDoUtf8String& in_style_set_name)
			: FSlateStyleSet(in_style_set_name)
		{
			//todo:add color implements
		}
	};

	std::shared_ptr<ISlateStyle> FUMGCoreStyle::create()
	{
		std::shared_ptr<FUMGStyleSet> style = std::make_shared<FUMGStyleSet>("UMGCoreStyle");
		style->set_content_root(FPaths::engine_content_dir() / "Slate");

		//todo:add checker board exist check

		//SButton defaults
		FSlateRoundedBoxBrush button_normal = { FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)), 4.0f, FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)), input_focus_thickness, Icon32x32 };
		FSlateRoundedBoxBrush button_hovered = { FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)), 4.0f, FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)), input_focus_thickness, Icon32x32 };
		FSlateRoundedBoxBrush button_pressed = { FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)), 4.0f, FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)), input_focus_thickness, Icon32x32 };

		static const FButtonStyle Button = FButtonStyle()
					.set_normal(button_normal)
					.set_hovered(button_hovered)
					.set_pressed(button_pressed);



		//SDockTab, SDockingTarget, SDockingTabStack defaults...
		{
			style->set("Docking.UnhideTabwellButton", FButtonStyle(Button)
				.set_normal(FSlateImageBrush(style->root_to_content_dir("Docking/ShowTabwellButton_Normal", ".png"), glm::vec2(10.0f, 10.0f)))
				.set_pressed(FSlateImageBrush(style->root_to_content_dir("Docking/ShowTabwellButton_Pressed", ".png"), glm::vec2(10.0f, 10.0f)))
				.set_hovered(FSlateImageBrush(style->root_to_content_dir("Docking/ShowTabwellButton_Hovered", ".png"), glm::vec2(10.0f, 10.0f)))
				.set_normal_padding(0)
				.set_pressed_padding(0)
			);
		}

		return style;
	}
	void FUMGCoreStyle::reset_to_default()
	{
		set_style(FUMGCoreStyle::create());
	}
	void FUMGCoreStyle::set_style(const std::shared_ptr<ISlateStyle>& new_style)
	{
		if (m_instance)
		{
			//todo:un register slate style
		}

		m_instance = new_style;

		if (m_instance)
		{
			FSlateStyleRegistry::register_slate_style(*m_instance);
		}
		else
		{
			reset_to_default();
		}
	}
}