#pragma once

#include "SlateCore/Widgets/IToolTip.h"

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo

#include "SlateCore/Styling/SlateBrush.h"//FSlateBrush depends on it

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

#include "SlateCore/Styling/ISlateStyle.h"//ISlateStyle depends on it

namespace DoDo {
	/*
	* slate tool tip widget
	*/
	class SToolTip : public SCompoundWidget, public IToolTip
	{
	public:
		DECLARE_DELEGATE_OneParam(FOnSetInteractiveWindowLocation, glm::vec2&);

		SLATE_BEGIN_ARGS(SToolTip)
			: _Text()
			, _Content()
			, _Font(FCoreStyle::get().get_font_style("ToolTip.Font"))
			, _TextMargin(FMargin(8.0f))
			, _BorderImage(FCoreStyle::get().get_brush("ToolTip.Background"))
			, _IsInteractive(false)
			, _OnSetInteractiveWindowLocation()
		{}
			/*the text displayed in this tool tip*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Text)

			/*arbitrary content to be displayed in the tool tip, overrides any text that may be set*/
			SLATE_DEFAULT_SLOT(FArguments, Content)

			/*the font to use for this tool tip*/
			SLATE_ATTRIBUTE(FSlateFontInfo, Font)

			/*margin between the tool tip border and the text content*/
			SLATE_ATTRIBUTE(FMargin, TextMargin)

			/*the background/border image to display*/
			SLATE_ATTRIBUTE(const FSlateBrush*, BorderImage)

			/*whether the tooltips should be considered interactive*/
			SLATE_ATTRIBUTE(bool, IsInteractive)

			/*hook to modify or override the desired location (in screen space) for interactive tooltip window, by default, the previous frames'cursor position will be used*/
			SLATE_EVENT(FOnSetInteractiveWindowLocation, OnSetInteractiveWindowLocation)
		SLATE_END_ARGS()

			/*
			* construct this widget
			* 
			* @param InArgs the declaration data for this widget
			*/
			void Construct(const FArguments& in_args);

	public:
		//IToolTip interface

		virtual std::shared_ptr<SWidget> as_widget() override;

		virtual std::shared_ptr<SWidget> get_content_widget() override;

		virtual void set_content_widget(const std::shared_ptr<SWidget>& in_content_widget) override;

		virtual bool is_empty() const override;

		virtual bool is_interactive() const override;

		virtual void on_opening() override;

		virtual void on_closed() override;

	private:
		//text block widget
		TAttribute<DoDoUtf8String> m_text_content;

		//content widget
		std::weak_ptr<SWidget> m_widget_content;

		//wrapped content within the widget
		std::shared_ptr<SWidget> m_tooltip_content;

		//font used for the text displayed (were applicable)
		TAttribute<FSlateFontInfo> m_font;

		//Margin between the tool tip border and the text content
		TAttribute<FMargin> m_text_margin;

		//the background/border image to display
		TAttribute<const FSlateBrush*> m_border_image;

		//whether the tooltip should be considered interactive
		TAttribute<bool> m_b_is_interactive;
		
		//optional delegate to modify or override the desired location for an interactive tooltip
		FOnSetInteractiveWindowLocation m_on_set_interactive_window_location_delegate;
	};
}