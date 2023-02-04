#pragma once

#include <map>

#include "ISlateStyle.h"

#include "Core/String/DoDoString.h"

#include "SlateColor.h"

namespace DoDo {
	class FSlateWidgetStyle;
	//class DoDoUtf8String;
	/*
	* a slate style chunk that contains a collection of named properties that guide the appearance of slate
	* at the moment, basically FEditorStyle
	*/
	class FSlateStyleSet : public ISlateStyle
	{
	public:
		/*
		* construct a style chunk
		* 
		* @param InStyleSetName the name used to identity this style set
		*/
		FSlateStyleSet(const DoDoUtf8String& in_style_set_name);

		virtual ~FSlateStyleSet();

		virtual void set_content_root(const DoDoUtf8String& in_content_root_dir);//content root directory

		virtual DoDoUtf8String root_to_content_dir(const char* relative_path, const char* extension);

		/*
		* add a FSlateLinearColor property to this style's collection
		* 
		* @param PropertyName - Name of the property to add
		* @param InColor - The Value to add
		*/
		void set(const DoDoUtf8String property_name, const FSlateColor& in_color)
		{
			m_slate_color_values.insert({ property_name, in_color });
		}

		template<typename DefinitionType>
		void set(const DoDoUtf8String property_name, const DefinitionType& in_style_definition)
		{
			m_widget_style_values.insert({ property_name, std::make_shared<DefinitionType>(in_style_definition) });//need to implement copy constructor
		}

		virtual const DoDoUtf8String& get_style_set_name() const override;
		
	protected:
		/*the name used to identity this style set*/
		DoDoUtf8String m_style_set_name;

		/*this dir is Engine/Editor Slate folder*/
		DoDoUtf8String m_content_root_dir;

		/*FSlateColor property storage*/
		std::map<DoDoUtf8String, FSlateColor> m_slate_color_values;

		//for button style or some widget style
		std::map < DoDoUtf8String, std::shared_ptr<FSlateWidgetStyle>> m_widget_style_values;
		
	};
}