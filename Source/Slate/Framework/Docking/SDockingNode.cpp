#include <PreCompileHeader.h>

#include "SDockingNode.h"

#include "SDockingSplitter.h"

namespace DoDo {
	std::shared_ptr<SDockingArea> SDockingNode::get_dock_area()
	{
		return m_parent_node_ptr.expired() ? m_parent_node_ptr.lock()->get_dock_area() : std::shared_ptr<SDockingArea>();
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
