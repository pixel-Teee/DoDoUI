#include <PreCompileHeader.h>

#include "SDockingNode.h"

namespace DoDo {
	float SDockingNode::get_size_coefficient() const
	{
		return m_size_coefficient;
	}
	void SDockingNode::set_size_coefficient(float in_size_coefficient)
	{
		m_size_coefficient = in_size_coefficient;
	}
}