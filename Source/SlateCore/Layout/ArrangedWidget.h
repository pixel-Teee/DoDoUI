#pragma once

#include "Geometry.h"

namespace DoDo
{
	class SWidget;
	/*
	 * a pair : widget and it's geometry
	 * widgets populate an list of widget geometries
	 *
	 * when they arrange their children, see SWidget::ArrangeChildren
	 */
	class FArrangedWidget
	{
	public:

		FArrangedWidget(std::shared_ptr<SWidget> in_widget, const FGeometry& in_geometry)
			: m_geometry(in_geometry)
			, m_widget(in_widget)
		{}

		/*
		 * the widget that is being arranged
		 */
		SWidget* get_widget_ptr() const
		{
			return m_widget.get();
		}
	public:
		/*the widget's geometry*/
		FGeometry m_geometry;

		/*the widget that is beging arranged*/
		std::shared_ptr<SWidget> m_widget;
	};

}
