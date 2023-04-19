#include <PreCompileHeader.h>

#include "SDockingNode.h"

#include "SDockingSplitter.h"

namespace DoDo {
	std::shared_ptr<SDockingArea> SDockingNode::get_dock_area()
	{
		//todo:fix me
		return m_parent_node_ptr.lock() != nullptr ? m_parent_node_ptr.lock()->get_dock_area() : std::shared_ptr<SDockingArea>();
	}

	std::shared_ptr<const SDockingArea> SDockingNode::get_dock_area() const
	{
		return m_parent_node_ptr.lock() != nullptr ? m_parent_node_ptr.lock()->get_dock_area() : std::shared_ptr<SDockingArea>();
	}

	FReply SDockingNode::on_user_attempting_dock(SDockingNode::RelativeDirection direction, const FDragDropEvent& drag_drop_event)
	{
		return FReply::un_handled();
	}

	float SDockingNode::get_size_coefficient() const
	{
		return m_size_coefficient;
	}
	void SDockingNode::set_size_coefficient(float in_size_coefficient)
	{
		m_size_coefficient = in_size_coefficient;
	}
}
