#pragma once

#include <memory>

#include "SlateCore/Layout/FlowDirection.h"

#include "SlateCore/Layout/Geometry.h"

#include "SlateCore/Styling/WidgetStyle.h"//FWidgetStyle depends on it
//FWidgetStyle just three color

namespace DoDo
{
	class SWidget;
	/*
	 * represents the state of a widget from when it last had SWidget::Paint called on it
	 * this should contain everything needed to directly call Paint on a widget
	 */

	//note:this is useful for next frame input handle
	struct FSlateWidgetPersistentState
	{
		FSlateWidgetPersistentState()
			: m_layer_id(0)
			, m_outgoing_layer_id(0)
			, m_incoming_user_index(-1)
			, m_incoming_flow_direction(EFlowDirection::LeftToRight)
			, m_b_parent_enabled(true)
		{}

		std::weak_ptr<SWidget> m_paint_parent;

		FGeometry m_allotted_geometry;

		FGeometry m_desktop_geometry;//note:this is will useful for panel widget

		FWidgetStyle m_widget_style;//note:this is different from FSlateWidgetStyle, it just three color

		//------starting layer id for drawing children------
		int32_t m_layer_id;

		int32_t m_outgoing_layer_id;

		int8_t m_incoming_user_index;

		EFlowDirection m_incoming_flow_direction;

		uint8_t m_b_parent_enabled : 1;
		//------starting layer id for drawing children------
	};
}
