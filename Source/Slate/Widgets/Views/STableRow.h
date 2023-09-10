#pragma once

#include "ITableRow.h"

#include "Slate/Widgets/Layout/SBorder.h"//SBoder depends on it

#include "Slate/Framework/Views/ITypedTableView.h"//ETableViewMode depends on it

#include "Slate/Widgets/Views/SExpanderArrow.h"

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
			set_owner_table_view(in_owner_table_view);

			Construct_Children(
				in_owner_table_view->m_table_view_mode, //interms of this view mode to construct different view
				in_args._Padding,
				in_args._Content.m_widget
			);
		}

		void set_owner_table_view(std::shared_ptr<STableViewBase> owner_table_view)
		{
			this->m_owner_table_ptr = std::static_pointer_cast<SListView<ItemType>>(owner_table_view);
		}

		virtual void Construct_Children(ETableViewMode::Type in_owner_table_mode, const TAttribute<FMargin>& in_padding, const std::shared_ptr<SWidget>& in_content)
		{
			this->m_content = in_content;

			if (in_owner_table_mode == ETableViewMode::List || in_owner_table_mode == ETableViewMode::Tile)
			{
				//we just need to hold on to this row's content
				this->m_child_slot
				.Padding(in_padding)
				[
					in_content
				];
			}
			else
			{
				//---row is for treeview---
				SHorizontalBox::FSlot* innner_content_slot_native_ptr = nullptr;

				//rows in a tree view need an expander button and some indentation

				this->m_child_slot
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.auto_width()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Fill)
					[
						SNew(SExpanderArrow, std::static_pointer_cast<STableRow>(shared_from_this()))
					]
					+ SHorizontalBox::Slot()
					.fill_width(1)
					.Expose(innner_content_slot_native_ptr)
					.Padding(in_padding)
					[
						in_content
					]
				];
			}
			
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

		virtual int32_t get_indent_level() const override
		{
			return m_owner_table_ptr.lock()->private_get_nesting_depth(m_index_in_list);
		}

		virtual void toggle_expansion() override
		{
			std::shared_ptr<ITypedTableView<ItemType>> owner_table = m_owner_table_ptr.lock();

			const bool b_item_has_children = owner_table->private_does_item_have_children(m_index_in_list);

			if (b_item_has_children)
			{
				if (const ItemType* my_item_ptr = get_item_for_this(owner_table))
				{
					const bool b_is_item_expanded = b_item_has_children && owner_table->private_is_item_expanded(*my_item_ptr);
					owner_table->private_set_item_expansion(*my_item_ptr, !b_is_item_expanded);
				}
			}
		}

		const ItemType* get_item_for_this(const std::shared_ptr<ITypedTableView<ItemType>>& owner_table) const
		{
			//std::shared_ptr<ITypedTableView<ItemType>> owner_table = m_owner_table_ptr.lock();

			const ItemType* my_item_ptr = owner_table->private_item_from_widget(this);
			if (my_item_ptr)
			{
				return my_item_ptr;
			}

			return nullptr;
		}

		virtual int32_t does_item_have_children() const override
		{
			return m_owner_table_ptr.lock()->private_does_item_have_children(m_index_in_list);
		}

		virtual bool is_item_expanded() const override
		{
			std::shared_ptr<ITypedTableView<ItemType>> owner_table = m_owner_table_ptr.lock();

			if (const ItemType* my_item_ptr = get_item_for_this(owner_table))
			{
				return owner_table->private_is_item_expanded(*my_item_ptr);
			}
		}

	protected:
		/*the list that owns this selectable*/
		std::weak_ptr<ITypedTableView<ItemType>> m_owner_table_ptr;

		/*index of the corresponding data item in the list*/
		int32_t m_index_in_list;

		/*the widget in the content slot for this row*/
		std::weak_ptr<SWidget> m_content;
	};
}