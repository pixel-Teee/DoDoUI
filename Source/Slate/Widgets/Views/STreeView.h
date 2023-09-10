#pragma once

#include "SListView.h"

#include <bitset>

namespace DoDo {
	/*info needed by a (relatively) small fraction of the tree items, some of them may not be visible*/
	struct FSparseItemInfo
	{
		FSparseItemInfo(bool in_is_expanded, bool in_has_expanded_children)
			: m_b_is_expanded(in_is_expanded)
			, m_b_has_expanded_children(in_has_expanded_children)
		{

		}

		/*is this tree item expanded*/
		bool m_b_is_expanded;

		/*does this tree item have any expanded children?*/
		bool m_b_has_expanded_children;
	};

	/*info needed by every visible item in the tree*/
	struct FItemInfo
	{
		FItemInfo()
		{

		}

		FItemInfo(std::vector<char> in_needs_vertical_wire, bool in_has_children, bool in_is_last_child, int32_t in_parent_index)
			: m_needs_vertical_wire(in_needs_vertical_wire)
			, m_b_has_children(in_has_children)
			, m_b_is_last_child(in_is_last_child)
			, m_parent_index(in_parent_index)
		{

		}

		/*
		* flags for whether we need a wire drawn for this level of the tree
		* 
		* needs vertical write.num() is the nesting level within the tree. e.g. 0 is root-level, 1 is children of root, etc.
		*/
		std::vector<char> m_needs_vertical_wire;//todo:fix me

		int32_t get_nesting_level() const { return m_needs_vertical_wire.size() - 1; }

		/*does this tree item have children?*/
		uint32_t m_b_has_children : 1;

		/*is this the last child of its parent? is so, it gets a special kind of wire/connector*/
		uint32_t m_b_is_last_child : 1;

		/*index into the linearized tree of the parent for this item, if any othewise index_none*/
		int32_t m_parent_index;
	};
	/*
	* this assumes you are familiar with SListView, see SListView
	* 
	* TreeView setup is virtually identical to that of ListView
	* 
	* additionally, TreeView introduces a new delegate: OnGetChildren()
	* OnGetChildren() takes some DataItem being observed by the tree
	* and return that item's children, Like ListView, TreeViem operates
	* exclusively with pointers to DataItems
	* 
	*/
	template<typename ItemType>
	class STreeView : public SListView<ItemType>
	{
	public:
		using TSparseItemMap = std::map<ItemType, FSparseItemInfo>;

		using FOnGetChildren = typename TSlateDelegates<ItemType>::FOnGetChildren;

		using FOnGenerateRow = typename TSlateDelegates<ItemType>::FOnGenerateRow;

		using TItemSet = std::set<ItemType>;

		SLATE_BEGIN_ARGS(STreeView)
			: _OnGetChildren()
			, _TreeItemSource()
			, _ScrollBarStyle(&FAppStyle::get().get_widget_style<FScrollBarStyle>("Scrollbar"))
		{}

			SLATE_EVENT(FOnGetChildren, OnGetChildren)

			SLATE_EVENT(FOnTableViewScrolled, OnTreeViewScrolled)
			
			SLATE_EVENT(FOnGenerateRow, OnGenerateRow)

			SLATE_ARGUMENT(float, ItemHeight)

			SLATE_STYLE_ARGUMENT(FScrollBarStyle, ScrollBarStyle)

			SLATE_ARGUMENT(std::shared_ptr<SHeaderRow>, HeaderRow)

			SLATE_ARGUMENT(std::shared_ptr<SScrollBar>, ExternalScrollbar)

			SLATE_ARGUMENT(const std::vector<ItemType>*, TreeItemSource)
		SLATE_END_ARGS()

		/*
			construct this widget
			@param InArgs the declaration data for this widget
		*/
		void Construct(const FArguments& in_args)
		{
			m_on_generate_row = in_args._OnGenerateRow;
			m_on_get_children = in_args._OnGetChildren;
			m_tree_items_source = in_args._TreeItemSource;

			//make the table view
			this->ConstructChildren(0, in_args._ItemHeight, EListItemAlignment::LeftAligned, in_args._HeaderRow, in_args._ExternalScrollbar, Orient_Vertical, in_args._OnTreeViewScrolled, in_args._ScrollBarStyle);
		}

		STreeView()
			: SListView<ItemType>(ETableViewMode::Tree)
			, m_tree_items_source(nullptr)
		{
			this->m_items_source = &m_linearized_items;//note:important
		}

		virtual int32_t private_get_nesting_depth(int32_t item_index_in_list) const override
		{
			int32_t nesting_level = 0;

			if (item_index_in_list >= 0 && item_index_in_list < m_dense_item_infos.size())
			{
				nesting_level = m_dense_item_infos[item_index_in_list].get_nesting_level();
			}

			return nesting_level;
		}
		
		virtual bool private_does_item_have_children(int32_t item_index_in_list) const override
		{
			bool b_has_children = false;
			//todo:add more logic

			if (item_index_in_list >= 0 && item_index_in_list < m_dense_item_infos.size())
			{
				b_has_children = m_dense_item_infos[item_index_in_list].m_b_has_children;
			}


			return b_has_children;
		}

		virtual bool private_is_item_expanded(const ItemType& the_item) const override
		{
			//todo:add more logic
			auto it = m_sparse_item_infos.find(the_item);

			return it != m_sparse_item_infos.end() && it->second.m_b_is_expanded;
		}

		virtual void private_set_item_expansion(ItemType the_item, bool b_should_be_expanded) override
		{
			auto sparse_item_info = m_sparse_item_infos.find(the_item);

			bool b_was_expanded = false;

			if (sparse_item_info != m_sparse_item_infos.end())
			{
				b_was_expanded = sparse_item_info->second.m_b_is_expanded;
				bool has_expanded_children = sparse_item_info->second.m_b_has_expanded_children;
				m_sparse_item_infos.erase(sparse_item_info);
				m_sparse_item_infos.insert({ the_item, FSparseItemInfo(b_should_be_expanded, has_expanded_children) });
			}
			else if (b_should_be_expanded)
			{
				m_sparse_item_infos.insert({ the_item, FSparseItemInfo(b_should_be_expanded, false) });
			}
			
			if (b_was_expanded != b_should_be_expanded)
			{
				//todo:call delegate

				request_tree_refresh();
			}
		}

		void request_tree_refresh()
		{
			request_list_refresh();
		}

		virtual void request_list_refresh() override
		{
			//todo:add more logic
			SListView<ItemType>::request_list_refresh();
		}
	public:

		virtual void Tick(const FGeometry& allotted_geometry, const double in_current_time, const float in_delta_time) override
		{
			if (this->m_items_panel)
			{
				if (m_on_get_children.is_bound() && m_tree_items_source != nullptr)
				{
					TItemSet temp_selected_items_map;
					TSparseItemMap temp_sparse_item_info;
					std::vector<FItemInfo> temp_dense_item_infos;

					m_linearized_items.clear();

					populate_linearized_items(*m_tree_items_source, m_linearized_items, temp_dense_item_infos, std::vector<char>(),
						temp_selected_items_map, temp_sparse_item_info, true, -1);

					m_sparse_item_infos = std::move(temp_sparse_item_info);
					m_dense_item_infos = std::move(temp_dense_item_infos);
				}
			}

			SListView<ItemType>::Tick(allotted_geometry, in_current_time, in_delta_time);
		}

		/*
		* given: an array of items (ItemsSource) each of which potentially has a child
		* 
		* task:populate the linearized items array with a flattened version of the visible data items
		* in the process, remove any items that are not visible while maintaining any collapsed
		* items that may have expanded children
		* 
		* 
		*/
		bool populate_linearized_items(
			const std::vector<ItemType>& in_items_source,
			std::vector<ItemType>& in_linearized_items,
			std::vector<FItemInfo>& new_dense_item_infos,
			std::vector<char> needs_parent_wire,
			TItemSet& out_new_selected_items,
			TSparseItemMap& new_sparse_item_info,
			bool b_addiing_items,
			int32_t paret_index
		)
		{
			needs_parent_wire.push_back(false);
			const int32_t nesting_dpeth_index = needs_parent_wire.size() - 1;

			bool b_saw_expanded_items = false;
			for (int32_t item_index = 0; item_index < in_items_source.size(); ++item_index)
			{
				const ItemType& cur_item = in_items_source[item_index];

				//find this items children
				std::vector<ItemType> child_items;
				m_on_get_children.execute(in_items_source[item_index], child_items);

				const bool b_has_children = child_items.size() > 0;

				const bool b_is_last_child = (item_index == in_items_source.size() - 1);
				needs_parent_wire[nesting_dpeth_index] = !b_is_last_child;

				//is this item expanded, does it have expanded children?
				auto cur_item_info = m_sparse_item_infos.find(cur_item);
				const bool b_is_expanded = (cur_item_info == m_sparse_item_infos.end()) ? false : cur_item_info->second.m_b_is_expanded;
				bool b_has_expanded_children = (cur_item_info == m_sparse_item_infos.end()) ? false : cur_item_info->second.m_b_has_expanded_children;

				//add this item to the linearized list and update the selection set
				if (b_addiing_items)
				{
					in_linearized_items.push_back(cur_item);

					new_dense_item_infos.push_back(FItemInfo(needs_parent_wire, b_has_children, b_is_last_child, paret_index));

					//todo:check selected
				}

				if (b_is_expanded || b_has_expanded_children)
				{
					const bool b_add_child_items = b_addiing_items && b_is_expanded;
					b_has_expanded_children = populate_linearized_items(child_items, in_linearized_items, new_dense_item_infos,
						needs_parent_wire, out_new_selected_items, new_sparse_item_info, b_add_child_items, in_linearized_items.size() - 1);
				}

				if (b_is_expanded || b_has_expanded_children)
				{
					new_sparse_item_info.insert({ cur_item, FSparseItemInfo(b_is_expanded, b_has_expanded_children) });
				}

				b_saw_expanded_items = b_saw_expanded_items || b_is_expanded || b_has_expanded_children;
			}

			return b_saw_expanded_items;
		}

		virtual std::shared_ptr<ITableRow> generate_new_widget(ItemType in_item)
		{
			if (m_on_generate_row.is_bound())
			{
				return m_on_generate_row.execute(in_item, std::static_pointer_cast<STableViewBase>(shared_from_this()));
			}
			else
			{
				std::shared_ptr<STableRow<ItemType>> new_list_item_widget =
					SNew(STableRow<ItemType>, std::static_pointer_cast<STableViewBase>(shared_from_this()))
					.Content()
					[
						SNew(STextBlock)
						.Text("OnGenerateWidget() not assigned")
					];
			}
		}

	protected:
		/*a pointer to the items being observed by the tree view*/
		const std::vector<ItemType>* m_tree_items_source;

		FOnGenerateRow m_on_generate_row;

		/*the delegate that is invoked whenever we need to gather an item's children*/
		FOnGetChildren m_on_get_children;

		/*info needed by a small fraction of tree items, some these are not visible to the user*/
		TSparseItemMap m_sparse_item_infos;

		/*info needed by every item in the linearized version of the tree*/
		std::vector<FItemInfo> m_dense_item_infos;

		/*
		* a linearized version of the items being observed by the tree view
		* note that we inherit from a list view, which we point at this linearized version of the tree
		*/
		std::vector<ItemType> m_linearized_items;
	};
}