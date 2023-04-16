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
	void SDockingSplitter::remove_child_at(int32_t index_of_child_to_remove)
	{
		//keep children consistent between the dock node list and the generic widget list in the SSplitter
		m_children.erase(m_children.begin() + index_of_child_to_remove);
		m_splitter->remove_at(index_of_child_to_remove);
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
	std::shared_ptr<FTabManager::FLayoutNode> SDockingSplitter::gather_persistent_layout() const
	{
		//assume that all the nodes were dragged out, and there's no meaningful layout data to be gathered
		bool b_have_layout_data = false;

		std::shared_ptr<FTabManager::FSplitter> persistent_node = FTabManager::new_splitter()
			->set_orientation(this->get_orientation())
			->set_size_coefficient(this->get_size_coefficient());

		for (int32_t child_index = 0; child_index < m_children.size(); ++child_index)
		{
			std::shared_ptr<FTabManager::FLayoutNode> persistent_child = m_children[child_index]->gather_persistent_layout();
			if (persistent_child)
			{
				b_have_layout_data = true;
				persistent_node->split(persistent_child);
			}
		}

		return (b_have_layout_data) ? persistent_node : std::shared_ptr<FTabManager::FLayoutNode>();
	}
	SDockingNode::ECleanupRetVal SDockingSplitter::most_responsibility(SDockingNode::ECleanupRetVal A, SDockingNode::ECleanupRetVal B)
	{
		return std::min(A, B);
	}
	SDockingNode::ECleanupRetVal SDockingSplitter::clean_up_nodes()
	{
		ECleanupRetVal this_node_purpose = NoTabsUnderNode;

		for (int32_t child_index = 0; child_index < m_children.size(); ++child_index)
		{
			const std::shared_ptr<SDockingNode>& child_node = m_children[child_index];

			const ECleanupRetVal child_node_purpose = child_node->clean_up_nodes();
			this_node_purpose = most_responsibility(this_node_purpose, child_node_purpose);

			switch (child_node->get_node_type())
			{
				case SDockingNode::DockTabStack:
				{
					std::shared_ptr<SDockingTabStack> child_as_stack = std::static_pointer_cast<SDockingTabStack>(child_node);
					if (child_node_purpose == NoTabsUnderNode)
					{
						//this child node presents no tabs and keeps no tab history
						remove_child_at(child_index);
					}
					else
					{
						//this child is useful, keep it and move on to the next element
						++child_index;
					}
				}
				break;

				case SDockingNode::DockSplitter:
				{
					std::shared_ptr<SDockingSplitter> child_as_splitter = std::static_pointer_cast<SDockingSplitter>(child_node);
					if (child_node_purpose == NoTabsUnderNode)
					{
						//child node no longer useful
						remove_child_at(child_index);
					}
					else
					{
						if (child_as_splitter->m_children.size() == 1 || (child_as_splitter->get_orientation() == this->get_orientation()))
						{
							const float grand_child_coefficient_scale = child_as_splitter->get_size_coefficient() / child_as_splitter->compute_child_coefficient_total();

							//child node is redundant
							remove_child_at(child_index);

							//copy the child nodes up one level
							for (int32_t grand_child_index = 0; grand_child_index < child_as_splitter->m_children.size(); ++grand_child_index)
							{
								const std::shared_ptr<SDockingNode> grand_child_node = child_as_splitter->m_children[grand_child_index];
								grand_child_node->set_size_coefficient(grand_child_node->get_size_coefficient() * grand_child_coefficient_scale);
								add_child_node(grand_child_node, child_index);
								++child_index;
							}
						}
						else
						{
							//keep the child node
							++child_index;
						}
					}
				}
				break;
			}
		}

		//at this point we may have ended up with a single splitter child
		//if so, remove it and adopt all its children
		if (this->m_children.size() == 1 && (this->m_children[0]->get_node_type() == SDockingNode::DockSplitter || this->m_children[0]->get_node_type() == SDockingNode::DockArea))
		{
			std::shared_ptr<SDockingSplitter> sole_child = std::static_pointer_cast<SDockingSplitter>(this->m_children[0]);

			this->remove_child_at(0);
			this->m_splitter->set_orientation(sole_child->get_orientation());

			const float grand_child_coefficient_scale = sole_child->get_size_coefficient() / sole_child->compute_child_coefficient_total();

			for (int32_t grand_child_index = 0; grand_child_index < sole_child->m_children.size(); ++grand_child_index)
			{
				const std::shared_ptr<SDockingNode>& grand_child = sole_child->m_children[grand_child_index];
				grand_child->set_size_coefficient(grand_child->get_size_coefficient() * grand_child_coefficient_scale);
				this->add_child_node(grand_child);
			}
	
		}

		if (this_node_purpose == HistoryTabsUnderNode)
		{
			//collapse the node because it only has tab history, not live tabs
			//note that dock areas should never collapse
			const bool b_is_dock_area = !(this->m_parent_node_ptr.lock());

			if (!b_is_dock_area)
			{
				set_visibility(EVisibility::Collapsed);
			}
		}

		return this_node_purpose;
	}
	float SDockingSplitter::compute_child_coefficient_total() const
	{
		float coefficient_total = 0.0f;
		for (int32_t child_index = 0; child_index < m_children.size(); ++child_index)
		{
			coefficient_total += m_children[child_index]->get_size_coefficient();
		}
		return coefficient_total;
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
