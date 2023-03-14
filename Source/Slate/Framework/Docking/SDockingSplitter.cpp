#include <PreCompileHeader.h>

#include "SDockingSplitter.h"

#include "Slate/Widgets/Layout/SSplitter.h"

namespace DoDo {
	void SDockingSplitter::Construct(const FArguments& in_args, const std::shared_ptr<FTabManager::FSplitter>& persistent_node)
	{
	}
	void SDockingSplitter::add_child_node(const std::shared_ptr<SDockingNode>& in_child, int32_t in_location)
	{
		//TAttribute<float>(in_child, &SDockingNode::get_size_coefficient);
		//keep children consistent between the dock node list and the generic widget list in the SSplitter
		this->m_splitter->add_slot(in_location)
		.Value(TAttribute<float>(in_child, &SDockingNode::get_size_coefficient))//todo:implement this bind
		.SizeRule(TAttribute<SSplitter::ESizeRule>(in_child, &SDockingNode::get_size_rule))
		[
			in_child
		];
		
		if (in_location == -1)
		{
			this->m_children.push_back(in_child);
		}
		else
		{
			this->m_children.insert(m_children.begin() + in_location, in_child);
		}

		//whatever node we added in, we are now its parent
		in_child->set_parent_node(std::static_pointer_cast<SDockingSplitter>(shared_from_this()));
	}

	const std::vector<std::shared_ptr<SDockingNode>>& SDockingSplitter::get_child_nodes() const
	{
		return m_children;
	}
}
