#include <PreCompileHeader.h>
#include "SlateStyle.h"

#include "SlateWidgetStyle.h"

namespace DoDo {
	FSlateStyleSet::FSlateStyleSet(const DoDoUtf8String& in_style_set_name)
		: m_style_set_name(in_style_set_name)
	{
	}
	FSlateStyleSet::~FSlateStyleSet()
	{
		//delete all allocated brush resources
		for (auto it = m_brush_resources.begin(); it != m_brush_resources.end(); ++it)
		{
			//check brush have uobject
			delete it->second;
		}
	}
	void FSlateStyleSet::set_content_root(const DoDoUtf8String& in_content_root_dir)
	{
		m_content_root_dir = in_content_root_dir;//set content root directory
	}
	DoDoUtf8String FSlateStyleSet::root_to_content_dir(const char* relative_path, const char* extension)
	{
		return m_content_root_dir / relative_path + extension;
	}
	const DoDoUtf8String& FSlateStyleSet::get_style_set_name() const
	{
		return m_style_set_name;
	}
	void FSlateStyleSet::get_resources(std::vector<const FSlateBrush*>& out_resources) const
	{
		//collection for this style's brush resources
		std::vector<const FSlateBrush*> slate_brush_resources;

		for (auto it = m_brush_resources.begin(); it != m_brush_resources.end(); ++it)//normal brush
		{
			slate_brush_resources.push_back(it->second);
		}

		for (auto it = m_widget_style_values.begin(); it != m_widget_style_values.end(); ++it)//widget style
		{
			it->second->get_resources(slate_brush_resources);
		}

		//append this style's resources to out resources
		out_resources.insert(out_resources.end(), slate_brush_resources.begin(), slate_brush_resources.end());
	}
	const FSlateBrush* FSlateStyleSet::get_brush(const DoDoUtf8String& property_name) const
	{
		auto it = m_brush_resources.find(property_name);
		const FSlateBrush* result = m_brush_resources.find(property_name)->second;

		//todo:check

		return result;
	}
}