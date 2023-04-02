#include <PreCompileHeader.h>

#include "SDockingSplitter.h"

#include "Slate/Widgets/Layout/SSplitter.h"

#include "Slate/Framework/Docking/SDockingTabStack.h"

namespace DoDo {
	void SDockingSplitter::Construct(const FArguments& in_args, const std::shared_ptr<FTabManager::FSplitter>& persistent_node)
	{
		//in docksplitter mode we just act as a thin shell around a splitter widget
		this->m_child_slot
		[
			SAssignNew(m_splitter, SSplitter)
			.Orientation(persistent_node->get_orientation())
		];

		this->set_size_coefficient(persistent_node->get_size_coefficient());
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
	std::shared_ptr<SDockingTabStack> SDockingSplitter::find_tab_stack_to_house_window_controls()
	{
		return std::static_pointer_cast<SDockingTabStack>(this->find_tab_stack(ETabStackToFind::UpperRight));
	}
	std::shared_ptr<SDockingTabStack> SDockingSplitter::find_tab_stack_to_house_window_icon()
	{
		return std::static_pointer_cast<SDockingTabStack>(this->find_tab_stack(ETabStackToFind::UpperLeft));
	}
	EOrientation SDockingSplitter::get_orientation() const
	{
		return m_splitter->get_orientation();
	}
	std::shared_ptr<SDockingNode> SDockingSplitter::find_tab_stack(ETabStackToFind find_me) const
	{
		auto find_first_visible_child = [=]()->std::shared_ptr<SDockingNode>
		{
			for (auto child_node : m_children)
			{
				if (child_node->get_visibility() == EVisibility::visible)
				{
					return child_node;
				}
			}

			//we might find and modify some invisible nodes, not a problem
			return m_children[0];
		};

		auto find_last_visible_child = [=]()->std::shared_ptr<SDockingNode>
		{
			for (int32_t i = m_children.size() - 1; i >= 0; --i)
			{
				const std::shared_ptr<SDockingNode>& child_node = m_children[i];
				if (child_node->get_visibility() == EVisibility::visible)
				{
					return child_node;
				}
			}

			//we might find and modify some invisible nodes, not a problem
			return m_children.back();
		};

		//we want the top-most node that is on the left-most or the right-most
		//in the case of left-most, just grab the first element until we hit a leaf
		//in the case of right-most grab the first element for vertical splits and the last for horizontal
		std::shared_ptr<SDockingNode> candidate = (find_me == ETabStackToFind::UpperLeft || this->get_orientation() == Orient_Vertical)
			? find_first_visible_child()
			: m_children.back();

		const SDockingNode::Type candidate_type = candidate->get_node_type();

		if (candidate_type == DockTabStack)
		{
			return candidate;
		}
		else if (candidate_type == DockArea || candidate_type == DockSplitter)
		{
			return std::static_pointer_cast<SDockingSplitter>(candidate)->find_tab_stack(find_me);
		}
		else
		{
			return candidate;
		}
	}
}
