#pragma once

namespace DoDo {
	class SWidget;
	/*
	* interface for table views to talk to their rows
	*/
	class ITableRow
	{
	public:

		/*
		* called when the row has been generated and associated with an item in the owning table
		* any attempts to access the item associated with the row prior to this (i.e. in Construct() will fail, as the association is not yet established in the owning table)
		*/
		virtual void initialize_row() = 0;

		/*
		* called when the row has been released from the owning table and is no longer associated with any items therein
		* only relevant if the row widgets are pooled or otherwise referenced/kept alive outside the owning table, otherwise, the row is destroyed
		*/
		virtual void reset_row() = 0;

		/*
		* @param InIndexInList the index of the item for which this widget was generated
		*/
		virtual void set_index_in_list(int32_t in_index_in_list) = 0;

		/*@return how nested the item associated with this row when it is in a tree view*/
		virtual int32_t get_indent_level() const = 0;

		/*toggle the expansion of the item associated with this row*/
		virtual void toggle_expansion() = 0;

		/*@return this table row as a widget*/
		virtual std::shared_ptr<SWidget> as_widget() = 0;

		/*@return does this item have children?*/
		virtual int32_t does_item_have_children() const = 0;

		/*@return true if the corresponding item is expanded, false otherwise*/
		virtual bool is_item_expanded() const = 0;
	};
}