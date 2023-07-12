#pragma once

#include "STableViewBase.h"//STableViewBase depends on it

#include "Slate/Framework/Views/ITypedTableView.h"//ETableViewMode::Type depends on it

#include "Slate/Framework/SlateDelegates.h"//FOnGenerateRow depeneds on it

namespace DoDo {
	template<typename ItemType>
	class SListView : public STableViewBase
	{
	public:
		using FOnGenerateRow = typename TSlateDelegates<ItemType>::FOnGenerateRow;

	public:
		SLATE_BEGIN_ARGS(SListView<ItemType>)
		: _OnGenerateRow()
		, _ListItemSource()
		, _ItemHeight(16)
		, _ExternalScrollbar()
		, _ScrollBarStyle(&FAppStyle::get().get_widget_style<FScrollBarStyle>("Scrollbar"))
		{}
			SLATE_EVENT(FOnGenerateRow, OnGenerateRow)

			SLATE_ARGUMENT(const std::vector<ItemType>*, ListItemSource)

			SLATE_ATTRIBUTE(float, ItemHeight)

			SLATE_ARGUMENT(std::shared_ptr<SHeaderRow>, HeaderRow)

			SLATE_ARGUMENT(std::shared_ptr<SScrollBar>, ExternalScrollbar)

			SLATE_ARGUMENT(EOrientation, Orientation)

			SLATE_EVENT(FOnTableViewScrolled, OnListViewScrolled)

			SLATE_STYLE_ARGUMENT(FScrollBarStyle, ScrollBarStyle)
		SLATE_END_ARGS()

		/*
		* Construct this widget
		* 
		* @param InArgs The declaration data for this widget
		*/
		void Construct(const typename SListView<ItemType>::FArguments& InArgs)
		{
			 this->m_on_generate_row = InArgs._OnGenerateRow;
			 this->m_items_source = InArgs._ListItemSource;

			 //todo:check on generate row and item source

			 //make the table view
			 ConstructChildren(0, InArgs._ItemHeight, EListItemAlignment::LeftAligned, InArgs._HeaderRow, InArgs._ExternalScrollbar, InArgs._Orientation, InArgs._OnListViewScrolled, InArgs._ScrollBarStyle);
		}

		SListView(ETableViewMode::Type in_list_mode = ETableViewMode::List)
			: STableViewBase(in_list_mode)
			, m_items_source(nullptr)
		{
			
		}

		/*@return how many items there are in the TArray being observed*/
		virtual int32_t get_num_items_being_observed() const override
		{
			return m_items_source == nullptr ? 0 : m_items_source->size();
		}

	protected:

		/*delegate to be invoked when the list needs to generate a new widget from a data item*/
		FOnGenerateRow m_on_generate_row;

		/*pointer to the array of data items that we are observing*/
		const std::vector<ItemType>* m_items_source;
	};

}