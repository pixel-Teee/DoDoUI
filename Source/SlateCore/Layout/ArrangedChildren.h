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
		//query visibility filter have parameter visibility
		bool Accepts(EVisibility in_visibility) const
		{
			return EVisibility::Does_Visibility_Pass_Filter(in_visibility, m_visibility_filter);
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
