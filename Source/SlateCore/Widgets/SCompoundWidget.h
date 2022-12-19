#pragma once

#include "SWidget.h"

#include "glm/glm.hpp"

namespace DoDo {
	class SCompoundWidget : public SWidget
	{
	public:
		//because TSlateAttribute's delete constructor, so SCompound Widget's default constructor could not use
		//SCompoundWidget();

		virtual ~SCompoundWidget();

		/*
			returns the size scaling factor for this widget
			return size scaling factor
		*/
		const glm::vec2 Get_Content_Scale() const
		{
			return m_content_scale_attribute.Get();
		}

		/*
			set the content scale for this widget
			param InCotentScale Content scaling factor
		 */
		void Set_Content_Scale(TAttribute<glm::vec2> In_Content_Scale)
		{
			m_content_scale_attribute.Assign(*this, std::move(In_Content_Scale));
		}

	private:

		//the layout scale to apply to this widget's contents, useful for animation
		TSlateAttribute<glm::vec2> m_content_scale_attribute;

		//the color and opacity to apply to this widget and all its descendants
		//todo:need to use linear color
		TSlateAttribute<glm::vec4> m_color_and_opacity_attribute;

		//optional foreground color that will be inherited by all of this widget's contents
		TSlateAttribute<glm::vec4> m_foreground_color_attribute;
	};
}