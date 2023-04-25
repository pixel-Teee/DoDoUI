#include "PreCompileHeader.h"

#include "EditorStyleSet.h"

#include "SlateCore/Styling/SlateStyleRegistry.h"//FSlateStyleRegistry depends on it

#include "SlateCore/Styling/ISlateStyle.h"//ISlateStyle depends on it

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

namespace DoDo {
	std::shared_ptr<ISlateStyle> FEditorStyle::m_instance = nullptr;

	void FEditorStyle::reset_to_default()
	{
		//set_style(FCoreStyle::create("EditorStyle"));

		//todo:implement
	}

	void FEditorStyle::set_style(const std::shared_ptr<ISlateStyle>& new_style)
	{
		if (m_instance != new_style)
		{
			if (m_instance)
			{
				//todo:from FSlateStyleRegistry to unregister slate style
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
}