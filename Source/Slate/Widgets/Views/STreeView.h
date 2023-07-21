#pragma once

#include "SListView.h"

namespace DoDo {
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

	};
}