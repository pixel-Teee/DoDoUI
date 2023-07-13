#pragma once

#include "STableViewBase.h"//STableViewBase depends on it

#include "Slate/Framework/Views/ITypedTableView.h"//ETableViewMode::Type depends on it

#include "Slate/Framework/SlateDelegates.h"//FOnGenerateRow depeneds on it

#include <cmath>

#include "ITableRow.h"

#include "STableRow.h"//STableRow depends on it

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
		, _Orientation(Orient_Vertical)
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
			, m_widget_generator(this)
		{
			
		}

		/*@return how many items there are in the TArray being observed*/
		virtual int32_t get_num_items_being_observed() const override
		{
			return m_items_source == nullptr ? 0 : m_items_source->size();
		}

		/*
		* given an data item, generate a widget corresponding to it
		* 
		* @param InItem the data item which to visualize
		* 
		* @return a new widget that represents the data item
		*/
		virtual std::shared_ptr<ITableRow> generate_new_widget(ItemType in_item)
		{
			if (m_on_generate_row.is_bound())
			{
				return m_on_generate_row.execute(in_item, std::static_pointer_cast<STableViewBase>(shared_from_this()));
			}
			else
			{
				//the programmer did not provide an on_generate_row() handler, let them know

				std::shared_ptr<STableRow<ItemType>> new_list_item_widget =
					SNew(STableRow<ItemType>, std::static_pointer_cast<STableViewBase>(shared_from_this()))
					.Content()
					[
						SNew(STextBlock)
						.Text("OnGenerateWidget() not assigned")
					];

				return new_list_item_widget;
			}
		}

		float generate_widget_for_item(const ItemType& cur_item, int32_t item_index, int32_t start_index, float layout_scale_multiplier)
		{
			//find a previously generated widget for this item, if one exists
			std::shared_ptr<ITableRow> widget_for_item = m_widget_generator.get_widget_for_item(cur_item);

			if (!widget_for_item)
			{
				//we couldn't find an existing widgets, meaning that this data item was not visible before
				//make a new widget for it
				widget_for_item = this->generate_new_widget(cur_item);
			}

			//it is useful to know the item's index that the widget was generated from
			//helps with even/odd coloring
			widget_for_item->set_index_in_list(item_index);

			//let the item generator know that we encoutered the current item and associated widget
			m_widget_generator.on_item_seen(cur_item, widget_for_item);

			//we rely on the widgets desired size in order to determine how many fit on screen
			const std::shared_ptr<SWidget> newly_generated_widget = widget_for_item->as_widget();
			//todo:mark prepass dirty
			newly_generated_widget->slate_prepass(layout_scale_multiplier);

			//we have a widget for this item, add it to the panel so that it is part of the ui
			if (item_index >= start_index)
			{
				//generating widgets downward
				this->append_widget(widget_for_item);
			}
			else
			{
				//backfilling widgets, going upward
				this->insert_widget(widget_for_item);
			}

			const bool b_is_visible = newly_generated_widget->get_visibility().is_visible();
			FTableViewDimensions generated_widget_dimensions(this->m_orientation, b_is_visible ? newly_generated_widget->get_desired_size() : glm::vec2(0.0f));

			return generated_widget_dimensions.m_scroll_axis;
		}

		/*
		* if there is a pending request to scroll an item into view, do so
		* 
		* @param ListViewGeometry the geometry of the list view, can be useful for centering the item
		*/
		virtual EScrollIntoViewResult scroll_into_view(const FGeometry& list_view_geometry) override
		{
			//todo:add more logic

			return EScrollIntoViewResult::Success;
		}

		/*update generate widgets for items as needed and clean up any widgets that are no longer needed
		* re-arrange the visible widget order as necessary
		*/
		virtual FReGenerateResults re_generate_items(const FGeometry& my_geometry) override
		{
			auto double_fractional = [](double Value)->double
			{
				//return Value - std::func(Value)
				double fraction;
				double integer;
				fraction = modf(Value, &integer);
				return fraction;
			};

			//clear all the items from our panel, we will re-add them in the correct order momentarily
			this->clear_widgets();

			//ensure that we always begin and clean up a generation pass
			FGenerationPassGuard generation_pass_guard(m_widget_generator);

			if (m_items_source && m_items_source->size() > 0)
			{
				//items in view, including fractional items
				float items_in_view = 0.0f;

				//total length of widgets generate so far (height for verticial lists, width for horizontal)
				float length_generated_so_far = 0.0f;

				//length of generated widgets that in the bounds of the view
				float view_length_used_so_far = 0.0f;

				//index of the item at which we start generating based on how far scrolled down we are
				//note that we must generate at least one item
				int32_t start_index = std::clamp((int32_t)(floor(m_current_scroll_offset)), 0, (int32_t)(m_items_source->size() - 1));

				//length of the first item that is generated, this item is at the location where the user requested we scroll
				float first_item_length = 0.0f;

				//generate widgets assuming scenario 
				bool b_has_filled_available_area = false;
				bool b_at_end_of_list = false;

				const float layout_scale_multiplier = my_geometry.get_accumulated_layout_transform().get_scale();
				FTableViewDimensions my_dimensions(this->m_orientation, my_geometry.get_local_size());

				for (int32_t item_index = start_index; !b_has_filled_available_area && item_index < m_items_source->size(); ++item_index)
				{
					const ItemType& cur_item = (*m_items_source)[item_index];

					//check pointer valid

					const float item_length = generate_widget_for_item(cur_item, item_index, start_index, layout_scale_multiplier);

					const bool b_is_first_item = item_index == start_index;

					if (b_is_first_item)
					{
						first_item_length = item_length;
					}

					//track the number of items in the view, including fractions
					if (b_is_first_item)
					{
						//the first item may not be fully visible (but cannot exceed 1)
						//(first item fraction scrolled into view) is the fraction of the item that is visible after taking into account anything that may be scrolled off the top/left of the list view
						const float first_item_fraction_scrolled_into_view = 1.0f - (float)std::max(double_fractional(m_current_scroll_offset), 0.0);

						//first item length scrolled into view is the length of the item, ignoring anything that is scrolled off the top/left of the list view
						const float first_item_length_scrolled_into_view = item_length * first_item_fraction_scrolled_into_view;

						//first item visible fraction is either: the visible item length as a fraction of the available list view length (if the item size is larger than the available size,
						//otherwise this will be > 1), or just first item fraction scrolled into view (which can never be > 1)
						const float first_item_visible_fraction = std::min(my_dimensions.m_scroll_axis / first_item_length_scrolled_into_view, first_item_fraction_scrolled_into_view);

						items_in_view += first_item_visible_fraction;
					}
					else if (view_length_used_so_far + item_length > my_dimensions.m_scroll_axis)
					{
						//the last item may not be fully visible either
						items_in_view += (my_dimensions.m_scroll_axis - view_length_used_so_far) / item_length;
					}
					else
					{
						items_in_view += 1;
					}

					length_generated_so_far += item_length;

					view_length_used_so_far += (b_is_first_item)
						? item_length * items_in_view // for the first item, items in view <= 1.0f
						: item_length;

					b_at_end_of_list = item_index >= m_items_source->size() - 1;

					if (b_is_first_item && view_length_used_so_far >= my_dimensions.m_scroll_axis)
					{
						//since there was no sum of floating points, make sure we correctly detect the case where one element
						//fills up the space
						b_has_filled_available_area = true;
					}
					else
					{
						//note: to account for accrued error from floating point truncation and addition in our sum of dimensions used
						//we pad the allotted axis just a little to be sure we have filled the available space
						const float float_precision_offset = 0.001f;
						b_has_filled_available_area = view_length_used_so_far >= my_dimensions.m_scroll_axis + float_precision_offset;
					}
				}

				//handle scenario b
				//we may have stopped because we got to the end of the items, but we may still have space to fill!

				//todo:add logic

				return FReGenerateResults(m_current_scroll_offset, length_generated_so_far, items_in_view, false);
			}
			

			return FReGenerateResults(0.0f, 0.0f, 0.0f, false);
		}

	private:
		friend class FWidgetGenerator;

		/*
		* a widget generator is a component responsible for creating widgets from data items
		* it also provides mapping from currently generated widgets to the data items which they
		* represent
		*/
		class FWidgetGenerator
		{
		public:
			FWidgetGenerator(SListView<ItemType>* in_owner_list)
				: m_owner_list(in_owner_list)
			{}

			/*
			* called at the beginning of the generation pass
			* begins track of which widgets were in view and which were not (so we can clean them up)
			* 
			* @param InNumDataItems the total number of data items being observed
			*/
			void on_begin_generation_pass()
			{
				//assume all the previosuly generated items need to be cleaned up
				m_items_to_be_clean_up = m_items_with_generated_widgets;
				m_items_with_generated_widgets.clear();
			}

			/*
			* called at the end of the generation pass
			* cleans up any widgets associated with items that were not in view this frame
			*/
			void on_end_generation_pass()
			{
				process_item_clean_up();

				validate_widget_generation();
			}

			void process_item_clean_up()
			{
				for (int32_t item_index = 0; item_index < m_items_to_be_clean_up.size(); ++item_index)
				{
					ItemType item_to_be_cleaned_up = m_items_to_be_clean_up[item_index];
					
					auto it = m_item_to_widget_map.find(item_to_be_cleaned_up);
					//const std::shared_ptr<ITableRow> find_result = it->second;

					if (it != m_item_to_widget_map.end())
					{
						const std::shared_ptr<ITableRow> widget_to_clean_up = it->second;

						m_item_to_widget_map.erase(item_to_be_cleaned_up);
						m_widget_map_to_item.erase(widget_to_clean_up.get());

						//todo:reset row

						widget_to_clean_up->reset_row();
					}
					//todo:check item to be cleaned up is ptr valid?
				}

				m_items_to_be_clean_up.clear();
			}

			void validate_widget_generation()
			{
				//todo:add logic
			}

			/*
			* keep track of every item and corresponding widget during a generation pass
			* 
			* @param InItem the data item which is in view
			* @param InGeneratedWidget the widget generated for this item, it may have been newly generated
			*/
			void on_item_seen(ItemType in_item, std::shared_ptr<ITableRow> in_generated_widget)
			{
				auto it = m_item_to_widget_map.find(in_item);

				const bool b_widget_is_newly_generated = (it == m_item_to_widget_map.end());

				if (it != m_item_to_widget_map.end())
				{
					std::shared_ptr<ITableRow> loop_up_result = it->second;

					//it's a newly generated item!
					m_item_to_widget_map.insert({ in_item, in_generated_widget });

					m_widget_map_to_item.insert({ in_generated_widget.get(), in_item });

					//now that the item-widget association is established, the generated row can fully initialize itself
					in_generated_widget->initialize_row();

					//todo:call on entry initialized delegate
				}

				//we should not clean up this item's widgets because it is in view
				auto it2 = std::find(m_items_to_be_clean_up.begin(), m_items_to_be_clean_up.end(), in_item);
				if (it2 != m_items_to_be_clean_up.end())
				{
					m_items_to_be_clean_up.erase(it2);
				}
				m_items_with_generated_widgets.push_back(in_item);
			}

			/*
			* find a widget for this item if it has already been constructed
			*
			* @param item the item for which to find the widget
			* @return a pointer to the corresponding widget if it exists, otherwise nullptr
			*/
			std::shared_ptr<ITableRow> get_widget_for_item(const ItemType& item) const
			{
				auto it = m_item_to_widget_map.find(item);

				if (it == m_item_to_widget_map.end())
					return nullptr;
				else
					return it->second;
			}

			/*we store a pointer to the owner list for error purposes, so when asserts occur we can report which list it happened for*/
			SListView<ItemType>* m_owner_list;

			/*map of data items to corresponding SWidgets*/
			std::map<ItemType, std::shared_ptr<ITableRow>> m_item_to_widget_map;

			/*map of swidgets to data items from which they were generated*/
			std::map<const ITableRow*, ItemType> m_widget_map_to_item;

			/*a set of items that currently have a generated widget*/
			std::vector<ItemType> m_items_with_generated_widgets;

			/*total number of data items the last time we performed a generation pass*/
			int32_t m_total_items_last_generation;

			/*items that need their widgets destroyed because they are no longer on screen*/
			std::vector<ItemType> m_items_to_be_clean_up;
		};

		struct FGenerationPassGuard
		{
			FWidgetGenerator& m_generator;
			FGenerationPassGuard(FWidgetGenerator& in_generator)
				: m_generator(in_generator)
			{
				//let the widget generator that we starting a pass so that it can keep track of data items and widgets
				m_generator.on_begin_generation_pass();
			}

			~FGenerationPassGuard()
			{
				//we have completed the generation pass, the widget generator will clean up unused widgets when it gose out of scope
				m_generator.on_end_generation_pass();

			}
		};

	protected:
		/*a widget generator component*/
		FWidgetGenerator m_widget_generator;

		/*delegate to be invoked when the list needs to generate a new widget from a data item*/
		FOnGenerateRow m_on_generate_row;

		/*pointer to the array of data items that we are observing*/
		const std::vector<ItemType>* m_items_source;
	};

}