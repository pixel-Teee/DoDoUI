#pragma once

namespace DoDo {
	class SWidget;
	/*
	* interface for tool tips
	*/
	class IToolTip
	{
	public:
		/*
		* gets the widget that this tool tip represents
		* 
		* @return the tool tip widget
		*/
		virtual std::shared_ptr<SWidget> as_widget() = 0;

		/*
		* gets the tool tip's content widget
		* 
		* @return the content widget
		*/
		virtual std::shared_ptr<SWidget> get_content_widget() = 0;

		/*
		* sets the tool tip's content widget
		* 
		* @param InContentWidget the new content widget to set
		*/
		virtual void set_content_widget(const std::shared_ptr<SWidget>& in_content_widget) = 0;

		/*
		* resets the tool tip to its default without a content widget
		*/
		virtual void reset_content_widget() {}

		/*
		* checks whether this tool tips has no content to display right now
		* 
		* @return true if the tool tip has no content to display, false otherwise
		*/
		virtual bool is_empty() const = 0;

		/*
		* checks whether this tool tip can be made interactive by the user (by holding ctrl)
		* 
		* @return true if it is an interactive tooltip, false otherwise
		*/
		virtual bool is_interactive() const = 0;

		/*
		* called when the tooltip widget is about to be requested for opening
		*/
		virtual void on_opening() = 0;

		/*
		* called when the tooltip widget is closed and the tooltip is no longer needed
		*/
		virtual void on_closed() = 0;

		/*
		* allows an interactive tooltip to adjust and/or change its window location on open,
		* as interactive tooltips don't update the tooltip window to follow the cursor position
		* 
		* @param InOutDesiredLocation on input, contains the previous cursor position (in screen coordinates) plus the standard
		*		tooltip offset (SlateDefs::TooltipOffsetFromMouse), this position can be modified and/or
		*		reset to either adjust the offset or otherwise set to a new position as the output value
		*/
	public:
		/*virtual destructor*/
		virtual ~IToolTip() {}
	};
}