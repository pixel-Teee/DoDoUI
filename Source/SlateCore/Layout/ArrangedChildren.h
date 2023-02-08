#pragma once

#include "ArrangedWidget.h"//FArrangedChildren depends on it

#include "Visibility.h"

namespace DoDo
{
	/*
	 * the results of an arrange children are always returned as an FArrangedChildren
	 *
	 * FArrangedChildren supports a filter that is useful for excluding widgets with unwanted visibilities
	 */
	class FArrangedChildren
	{
	private:
		EVisibility m_visibility_filter;

	public:
		typedef std::vector<FArrangedWidget> FArrangedWidgetArray;

		FArrangedChildren(EVisibility in_visibility_filter, bool b_in_allow_3d_widgets = false)
			: m_visibility_filter(in_visibility_filter)
			, m_b_allow_3d_widgets(b_in_allow_3d_widgets)
		{
			
		}

		static FArrangedChildren hittest2_from_array(std::vector<FWidgetAndPointer>& in_widgets) //todo:move?
		{
			FArrangedChildren temp(EVisibility::All);
			//temp.m_array.resize(in_widgets.size());
			//todo:optimize this
			for(const FWidgetAndPointer& widget_and_pinter : in_widgets)
			{
				temp.m_array.push_back(widget_and_pinter);
			}

			return temp;
		}

		//query visibility filter have parameter visibility
		bool Accepts(EVisibility in_visibility) const
		{
			return EVisibility::Does_Visibility_Pass_Filter(in_visibility, m_visibility_filter);
		}

		/*
		 * add an arranged widget(i.e. widget and its resulting geometry) to the list of arranged children
		 *
		 * @param VisibilityOverride the arranged function may override the visibility of the widget for the purposes
		 *							of layout or performances (i.e. prevent redundant call to widet->get_visibility())
		 * @param InWidgetGeometry the arranged widget (i.e. widget and it's geometry)
		 */
		void add_widget(EVisibility visiblity_override, const FArrangedWidget& in_widget_geometry)
		{
			if(Accepts(visiblity_override))
			{
				m_array.push_back(in_widget_geometry);
			}
		}
	public:
		
	private:
		/*internal representation of the array widgets*/
		FArrangedWidgetArray m_array;

		bool m_b_allow_3d_widgets;

	public:
		int32_t num() const
		{
			return m_array.size();
		}

		const FArrangedWidget& operator[](int32_t index) const
		{
			return m_array[index];
		}

		FArrangedWidget& operator[](int32_t index)
		{
			return m_array[index];
		}
	}; 
}
