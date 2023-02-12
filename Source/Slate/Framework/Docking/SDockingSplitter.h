#pragma once

#include "SDockingNode.h"

#include "TabManager.h"

namespace DoDo {
	class SSplitter;
	//class FTabManager;
	//class FTabManager::FSplitter;
	/*dynamic n-way splitter that provides the resizing functionality in the docking framework*/
	class SDockingSplitter : public SDockingNode
	{
	public:
		SLATE_BEGIN_ARGS(SDockingSplitter)
		{}
		SLATE_END_ARGS()

		//todo:implement FTabManager FSplitter
		void Construct(const FArguments& in_args, const std::shared_ptr<FTabManager::FSplitter>& persistent_node);

		virtual Type get_node_type() const override
		{
			return SDockingNode::DockSplitter;//dock splitter
		}

		/*
		* add a new child dock node at the desired location
		* assumes this dock node is a splitter
		* 
		* @param InChild the dock node child to add
		* @param InLocation index at which to add, INDEX_NONE adds to the end of this list
		*/
		void add_child_node(const std::shared_ptr<SDockingNode>& in_child, int32_t in_location = -1);

	protected:
		//todo:implement these functions

		enum class ETabStackToFind
		{
			UpperLeft,
			UpperRight
		};

		/*helper: finds the upper left or the upper right tab stack in the hierarchy*/


		/*the SSplitter widget that SDockingSplitter wraps*/
		std::shared_ptr<SSplitter> m_splitter;

		/*the dock node children, all these children are kept in sync with the SSplitter's children via the use of the public interface for adding, removing and replacing children*/
		std::vector<std::shared_ptr<SDockingNode>> m_children;
	};
}