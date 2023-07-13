#pragma once

#include "ITableRow.h"

#include "Slate/Widgets/Layout/SBorder.h"//SBoder depends on it

#include "Slate/Framework/Views/ITypedTableView.h"//ETableViewMode depends on it

namespace DoDo {
	class STableViewBase;
	/*
	* the list view is populated by selectable widgets
	* a selectable widget is a way of the list view containing it (owner table) and holds arbitrary content (content)
	* a selectable works with its corresponding list view to provide selection functionality
	*/
	template<typename ItemType>
	class STableRow : public ITableRow, public SBorder
	{
	public:
		SLATE_BEGIN_ARGS(STableRow<ItemType>)
			: _ExpanderStyleSet(&FCoreStyle::get())
			, _Padding(FMargin(0))
			, _Content()
			{}
			SLATE_ARGUMENT(const ISlateStyle*, ExpanderStyleSet)

			SLATE_ATTRIBUTE(FMargin, Padding)

			SLATE_DEFAULT_SLOT(typename STableRow<ItemType>::FArguments, Content)
		SLATE_END_ARGS()

		virtual void reset_row() override {}

		void Construct(const typename STableRow<ItemType>::FArguments& in_args, const std::shared_ptr<STableViewBase>& in_owner_table_view)
		{
			Construct_Children(
				in_owner_table_view->m_table_view_mode,
				in_args._Padding,
				in_args._Content.m_widget
			);
		}

		virtual void Construct_Children(ETableViewMode::Type in_owner_table_mode, const TAttribute<FMargin>& in_padding, const std::shared_ptr<SWidget>& in_content)
		{
			this->m_content = in_content;

			this->m_child_slot
			.Padding(in_padding)
			[
				in_content
			];
		}

		virtual void set_index_in_list(int32_t in_index_in_list) override
		{
			m_index_in_list = in_index_in_list;
		}

		virtual void initialize_row() override
		{

		}

		virtual std::shared_ptr<SWidget> as_widget() override
		{
			return shared_from_this();
		}

	protected:
		/*index of the corresponding data item in the list*/
		int32_t m_index_in_list;

		/*the widget in the content slot for this row*/
		std::weak_ptr<SWidget> m_content;
	};
}