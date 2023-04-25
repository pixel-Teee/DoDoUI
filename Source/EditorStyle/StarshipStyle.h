#pragma once

#include "EditorStyle/EditorStyleSet.h"//FEditorStyle depends on it

#include "SlateCore/Styling/SlateStyle.h"//FSlateStyleSet depends on it

namespace DoDo {
	/*
	* declares the editor's visual style
	*/
	class FStarshipEditorStyle : public FEditorStyle
	{
	public:
		static void initialize();

		static void shut_down();

		class FStyle : public FSlateStyleSet
		{
		public:
			FStyle();

			void initialize();

			void set_up_graph_editor_styles();
		};

		static std::shared_ptr<FStarshipEditorStyle::FStyle> Create()
		{
			std::shared_ptr<FStarshipEditorStyle::FStyle> new_style = std::make_shared<FStarshipEditorStyle::FStyle>();

			new_style->initialize();//this is important

			return new_style;
		}

		static std::shared_ptr<FStarshipEditorStyle::FStyle> m_style_instance;
	};
}