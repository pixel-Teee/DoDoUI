#pragma once

#include <map>

#include "ISlateStyle.h"

#include "Core/String/DoDoString.h"

#include "SlateColor.h"

namespace DoDo {
	struct FSlateWidgetStyle;
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

		virtual FSlateBrush* get_default_brush() const override;

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

		template<typename BrushType>
		void set(const DoDoUtf8String& property_name, BrushType* in_brush)//note:the difference between DefinitionType, is this is pointer, we should to free this
		{
			m_brush_resources.insert({ property_name, in_brush });
		}

		virtual const DoDoUtf8String& get_style_set_name() const override;

		//populate to the parameter
		virtual void get_resources(std::vector<const FSlateBrush*>& out_resources) const override;

		virtual const FSlateBrush* get_brush(const DoDoUtf8String& property_name) const override;

		virtual const FSlateWidgetStyle* get_widget_style_internal(const DoDoUtf8String desired_type_name, const DoDoUtf8String style_name
		) const override;
		
	protected:
		/*the name used to identity this style set*/
		DoDoUtf8String m_style_set_name;

		/*this dir is Engine/Editor Slate folder*/
		DoDoUtf8String m_content_root_dir;

		/*FSlateColor property storage*/
		std::map<DoDoUtf8String, FSlateColor> m_slate_color_values;

		//for button style or some widget style
		std::map<DoDoUtf8String, std::shared_ptr<FSlateWidgetStyle>> m_widget_style_values;

		/*FSlateBrush property storage*/
		FSlateBrush* m_default_brush;
		std::map<DoDoUtf8String, FSlateBrush*> m_brush_resources;//we have life time of FSlateBrush
	};
}