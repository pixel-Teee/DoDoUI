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
		{}
			SLATE_EVENT(FOnGenerateRow, OnGenerateRow)

			SLATE_ARGUMENT(const std::vector<ItemType>*, ListItemSource)
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
		}

		SListView(ETableViewMode::Type in_list_mode = ETableViewMode::List)
			: STableViewBase(in_list_mode)
			, m_items_source(nullptr)
		{
			
		}

	protected:

		/*delegate to be invoked when the list needs to generate a new widget from a data item*/
		FOnGenerateRow m_on_generate_row;

		/*pointer to the array of data items that we are observing*/
		const std::vector<ItemType>* m_items_source;
	};
}