#include "PreCompileHeader.h"

#include "MenuStack.h"

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "SlateCore/Types/SlateStructs.h"//FOptionalSize depends on it

#include "Slate/Widgets/Layout/SBox.h"//SBox depends on it

#include "SlateCore/Widgets/SOverlay.h"//SOverlay depends on it

#include "SlateCore/Widgets/Images/SImage.h"//SImage depends on it

#include "Slate/Widgets/Layout/SBorder.h"

namespace MenuStackInternal
{
	using namespace DoDo;
	/*
	* widget that wraps any menu created in FMenuStack to provide default key handling, focus tracking and helps us spot menus in widget paths
	*/
	class SMenuContentWrapper : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SMenuContentWrapper)
			: _MenuContent()
			//, _OnKeyDown()
			, _OptionalMinMenuWidth()
			, _OptionalMinMenuHeight()
		{}
			SLATE_DEFAULT_SLOT(FArguments, MenuContent) //note:this is menu
				//SLATE_EVENT(FOnKeyDown, OnKeyDown)
				//SLATE_EVENT(FOnMenuLostFocus, OnMenuLostFocus)
			SLATE_ARGUMENT(FOptionalSize, OptionalMinMenuWidth)
			SLATE_ARGUMENT(FOptionalSize, OptionalMinMenuHeight)
		SLATE_END_ARGS()
		/*construct this widget*/
		void Construct(const FArguments& in_args)
		{
			//the visibility of the content wrapper should match that of the provided content

			//todo:set visibility

			m_child_slot
			[
				SNew(SBox)
				.MinDesiredWidth(in_args._OptionalMinMenuWidth)
				.MaxDesiredHeight(in_args._OptionalMinMenuHeight)
				[
					//always add a background to the menu, this includes a small outline around the background to distinguish open menus from each other
					SNew(SOverlay)
					+ SOverlay::Slot()
					[
						SNew(SImage)
						.Image(FCoreStyle::get().get_brush("Menu.Background"))
					]
					+ SOverlay::Slot()
					[
						SNew(SImage)
						.Image(FCoreStyle::get().get_brush("Menu.Background"))//todo:implement get optional brush
					]
					+ SOverlay::Slot()
					[
						SNew(SBorder)
						.Padding(0.0f)
						.BorderImage(FStyleDefaults::get_no_brush())//todo:implement SBorder default foreground
						[
							in_args._MenuContent.m_widget
						]
					]
				]
			];
		}
	};
}