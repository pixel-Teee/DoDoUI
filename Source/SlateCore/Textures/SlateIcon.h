#pragma once

#include "Core/String/DoDoString.h"

#include "SlateCore/Styling/SlateBrush.h"//FSlateBrush depends on it

#include "SlateCore/Styling/ISlateStyle.h"//ISlateStyle depends on it

namespace DoDo {
	/*
	* struct used to represent an icon in slate
	*/
	struct FSlateIcon
	{
		/*
		* default constructor (empty icon)
		*/
		FSlateIcon();

		/*
		* creates and initializes a new icon from a style set and style name
		* 
		* @param InStyleSetName the name of the style set the icon can be found in
		* @param StyleName the name of the style for the icon
		* @param InSmallStyleName the name of the style for the small icon
		* @param InStatusOverlayStyleName the name of the style for a status icon to overlay on the base image
		*/
		FSlateIcon(const DoDoUtf8String in_style_set_name, const DoDoUtf8String in_style_name, const DoDoUtf8String in_small_style_name = "",
			const DoDoUtf8String in_status_overlay_style_name = "");

		/*
		* gets the resolved icon
		* 
		* @return icon brush, or FStyleDefaults::get no brush() if the icon wasn't found
		* @see get small icon
		*/
		const FSlateBrush* get_icon() const;

		/*
		* gets the resolved style set
		* 
		* @return style set, or nullptr if the style set wasn't found
		* @see get_small_style_name, get_style_name, get_style_set_name
		*/
		const ISlateStyle* get_style_set() const;
	private:

		//the name of the style set the icon can be found in
		DoDoUtf8String m_style_set_name;

		//the name of the style for the icon
		DoDoUtf8String m_style_name;

		//the name of the style for the small icon
		DoDoUtf8String m_small_style_name;

		//name of the style for the status overlay icon
		DoDoUtf8String m_status_overlay_style_name;
	};
}