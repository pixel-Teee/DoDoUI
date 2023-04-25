#include "PreCompileHeader.h"

#include "StarshipStyle.h"

#include "Core/Misc/Paths.h"//FPaths depends on it

namespace DoDo {
	std::shared_ptr<FStarshipEditorStyle::FStyle> FStarshipEditorStyle::m_style_instance = nullptr;

	void FStarshipEditorStyle::initialize()
	{
		//todo:call slate application initialize core style

		m_style_instance = Create();

		set_style(m_style_instance); //note:register to style center
	}

	void FStarshipEditorStyle::shut_down()
	{
		m_style_instance.reset();
	}
	
	FStarshipEditorStyle::FStyle::FStyle()
		: FSlateStyleSet("EditorStyle")
	{
	}

	void FStarshipEditorStyle::FStyle::initialize()
	{
		set_content_root(FPaths::engine_content_dir() / "Editor/Slate");

		set_up_graph_editor_styles();
	}

	void FStarshipEditorStyle::FStyle::set_up_graph_editor_styles()
	{
	}

}