#include <PreCompileHeader.h>

#include "SToolTip.h"

#include "Slate/Widgets/Layout/SBorder.h"

#include "Slate/Widgets/Text/STextBlock.h"

namespace DoDo {

	void SToolTip::Construct(const FArguments& in_args)
	{
		m_text_content = in_args._Text;
		m_b_is_interactive = in_args._IsInteractive;
		m_font = in_args._Font;
		m_text_margin = in_args._TextMargin;
		m_border_image = in_args._BorderImage;
		m_on_set_interactive_window_location_delegate = in_args._OnSetInteractiveWindowLocation;

		set_content_widget(in_args._Content.m_widget);
	}

	std::shared_ptr<SWidget> SToolTip::as_widget()
	{
		return shared_from_this();
	}

	std::shared_ptr<SWidget> SToolTip::get_content_widget()
	{
		return m_tooltip_content;
	}

	void SToolTip::set_content_widget(const std::shared_ptr<SWidget>& in_content_widget)
	{
		if (in_content_widget != SNullWidget::NullWidget)
		{
			//widget content argument takes precedence over the text content
			m_widget_content = in_content_widget;
		}

		std::shared_ptr<SWidget> pinned_widget_content = m_widget_content.lock();

		if (pinned_widget_content)
		{
			m_tooltip_content = pinned_widget_content;

			//tool-tip with entirely custom content, we'll create a border with some padding (as customized by the user)
			//then embed their custom widget right inside the border, this tool-tip currently has different styling than tool-tips
			//that only contain text
			m_child_slot
			[
				SNew(SBorder)
				.BorderImage(m_border_image)
				.Padding(m_text_margin)
				[
					m_tooltip_content
				]
			];
		}
		else
		{
			m_tooltip_content =
				SNew(STextBlock)
				.Text(m_text_content)
				.Font(m_font)
				.ColorAndOpacity(FLinearColor::Black);

			//text-only tool-tip, this tool-tip currently has a different styling than tool-tips with custom content
			//we always want basic text tool-tip to look consistent
			m_child_slot
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::get().get_brush("ToolTip.BrightBackground"))
				.Padding(FMargin(11.0f))
				[
					m_tooltip_content
				]
			];
		}
	}

	bool SToolTip::is_empty() const
	{
		return !m_widget_content.lock() && m_text_content.Get().get_length() == 0;
	}

	bool SToolTip::is_interactive() const
	{
		return m_b_is_interactive.Get();
	}

	void SToolTip::on_opening()
	{
	}

	void SToolTip::on_closed()
	{

	}
}