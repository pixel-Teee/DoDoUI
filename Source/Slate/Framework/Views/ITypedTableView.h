#pragma once

namespace DoDo {
	/*lists can exist as regular lists or as backing widgets for a tree*/
	namespace ETableViewMode
	{
		enum Type
		{
			List,
			Tile,
			Tree
		};
	}

	/*Interface for container widgets to talk to the owner list, tree, or grid*/
	template<typename ItemType>
	class ITypedTableView
	{
	public:


	};
}