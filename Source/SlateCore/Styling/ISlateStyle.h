#pragma once

#include <vector>

#include "SlateBrush.h"

//#include "Core/String/DoDoString.h"

#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo depends on it

#include "SlateWidgetStyle.h"

#include "SlateCore/Styling/SlateDefaults.h"//FStyleDefaults depends on it

namespace DoDo {
	class DoDoUtf8String;
	//class FSlateBrush;
	class ISlateStyle
	{
	public:
		/*constructor*/
		ISlateStyle() {}

		/*@return the name used to identity this style set*/
		virtual const DoDoUtf8String& get_style_set_name() const = 0;

		/*populate an array of FSlateBrush with resources consumed by this style
		* @param OutResources - the array to populate
		*/
		virtual void get_resources(std::vector<const FSlateBrush*>& out_resources) const = 0;

		virtual const FSlateBrush* get_brush(const DoDoUtf8String& property_name) const = 0;//todo:implement this add extra parameter

		template<typename WidgetStyleType>
		const WidgetStyleType& get_widget_style(DoDoUtf8String property_name) const//todo:add extra parameter
		{
			//dispatch to specific class function
			const FSlateWidgetStyle* existing_style = get_widget_style_internal(WidgetStyleType::TypeName, property_name);

			//todo:check

			return *static_cast<const WidgetStyleType*>(existing_style);
			//return nullptr;
		}

		virtual const FSlateWidgetStyle* get_widget_style_internal(const DoDoUtf8String desired_type_name, const DoDoUtf8String style_name
			) const = 0;

		virtual FSlateFontInfo get_font_style(const DoDoUtf8String& property_name) const = 0;

		/*destructor*/
		virtual ~ISlateStyle();

		/*
		* get default FSlateBrush
		* 
		* @return default slate brush value
		*/
		virtual FSlateBrush* get_default_brush() const = 0;

		/*
		* just like get brush, but returns default brush instead of the "missing brush" image when the resource is not found
		* 
		* @param PropertyName name of the property to get
		* 
		* @param Specifier an optional string to append to the property name
		* 
		* @return an FSlateBrush property
		*/
		virtual const FSlateBrush* get_optional_brush(const DoDoUtf8String property_name, const char* specifier = nullptr,
			const FSlateBrush* const default_brush = FStyleDefaults::get_no_brush()) const = 0;
	};
}