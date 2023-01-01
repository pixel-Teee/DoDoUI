#pragma once

#include "Core/Math/IntPoint.h"
#include "glm/vec2.hpp"

namespace DoDo
{
	class SWidget;
	//todo:inherited from FNoncopyable
	class FHittestGrid
	{
	public:
		FHittestGrid();

		/*
		 * given a slate units coordinate in virtual desktop space, perform a hittest
		 * and return the path along which the corresponding event would be bubbled
		 */
		//todo:implement FWidgetAndPointer
		//return the path's FWidgetAndPointer Array
		//todo:implement GetBubblePath function

		/*
		 * set the position and size of the hittest are in desktop coordinates
		 *
		 * @param HittestPositionInDesktop the position of this hit testing area in desktop coordinates
		 * @param HittestDimensions the dimensions of this hit testing area
		 *
		 * @return returns true if a clear of the hittest grid was required
		 */
		bool Set_Hittest_Area(const glm::vec2& hittest_position_in_desktop, const glm::vec2& hittest_dimensions, const glm::vec2& hittest_offset_in_window = glm::vec2(0.0f));

		//todo:implement insert custom hit test path

		/*sets the current slate user index that should be associated with any added widgets*/
		void set_user_index(int32_t user_index) { m_current_user_index = user_index; }

		/*set the culling rect to be used by the parent grid(in case we are appended to another grid)*/
		//todo:implement FSlateRect

		/*set the owner SWidget to be used by the parent grid(in case we are appended to another grid)*/
		void set_owner(const SWidget* in_owner) { m_owner = in_owner; }

		/*gets current slate user index that should be associated with any added widgets*/
		int32_t get_user_index() const { return m_current_user_index; }

		/*
		 * finds the next focusable widget by searching through the hit test grid
		 */
		//todo:implement FindNextFocusableWidget

		glm::vec2 get_grid_size() const { return m_grid_size; }

		glm::vec2 get_grid_origin() const { return m_grid_origin; }

		/*clear the grid*/
		void clear();

		/*add SWidget from the hittest grid*/
		//todo:implement FSlateInvalidationWidgetSortOrder
		//todo:implement AddWidget

		/*remove SWidget from the hittest grid*/
		void remove_widget(const SWidget* in_widget);

		/*update the widget Secondary without removing it and readding it again*/
		//todo:implement FSlateInvalidationWidgetSortOrder

		/*
		 * add an already existing grid that occupy the same space
		 * the grid needs to have an owner, not be this grid and occupy the same space this grid
		 */
		void add_grid(const std::shared_ptr<const FHittestGrid>& other_grid);

		/*
		 * remove a grid that was appended
		 */
		void remove_grid(const std::shared_ptr<const FHittestGrid>& other_grid);


	private:
		/*
		 * widget data we maintain internally store along with the widget reference
		 */
		struct FWidgetData
		{
			std::weak_ptr<SWidget> m_weak_widget;

			//todo:implement ICustomHitTestPath
			FIntPoint m_upper_left_cell;

			FIntPoint m_lower_right_cell;

			int64_t m_primary_sort;

			//todo:implement FSlateInvalidationWidgetSortOrder

			int32_t m_user_index;

			std::shared_ptr<SWidget> get_widget() const { return m_weak_widget.lock(); }
		};

		struct FWidgetIndex
		{
			FWidgetIndex()
				: m_grid(nullptr)
				, m_widget_index(-1)//-1 is INDEX_NONE
			{}

			FWidgetIndex(const FHittestGrid* in_hittest_grid, int32_t in_index)
				: m_grid(in_hittest_grid)
				, m_widget_index(in_index)
			{}

		private:
			const FHittestGrid* m_grid;

			int32_t m_widget_index;
		};

		struct FIndexAndDistance : FWidgetIndex
		{
			FIndexAndDistance()
				: FWidgetIndex()
				, m_distance_sq_to_widget(0)
			{}

			FIndexAndDistance(FWidgetIndex widget_index, float in_distance_sq)
				: FWidgetIndex(widget_index)
				, m_distance_sq_to_widget(in_distance_sq)
			{}

			float get_distance_sq_to_widget() const { return m_distance_sq_to_widget; }
		private:
			float m_distance_sq_to_widget;
		};

		/*
		 * all the available space is partitioned into cells
		 * each cell contains a list of widgets that overlap the cell
		 * the list is ordered from back to front
		 */
		struct FCell
		{
		public:
			FCell() = default;

			void add_index(int32_t widget_index);
			void remove_index(int32_t widget_index);

			const std::vector<int32_t>& get_widget_indexes() const { return m_widget_indexes; }
		private:
			std::vector<int32_t> m_widget_indexes;
		};

		struct FAppendGridData
		{
			FAppendGridData(const SWidget* in_cached_owner, const std::weak_ptr<const FHittestGrid>& in_grid)
				: m_cached_owner(in_cached_owner), m_grid(in_grid)
			{
				
			}

			const SWidget* m_cached_owner;//cached owner of the grid

			std::weak_ptr<const FHittestGrid> m_grid;
		};

	private:
		/*map of all the widget currently in the hit test grid to their stable index*/
		std::map<const SWidget*, int32_t> m_widget_map;

		/*stable indexed sparse array of all the widget data we track*/
		//todo:implement TSpareArray
		std::vector<FWidgetData> m_widget_array;

		/*the celss that make up the space partition*/
		std::vector<FCell> m_cells;

		/*the collapsed grid cached untitled it's dirtied*/
		std::vector<FAppendGridData> m_append_grid_array;

		/*a grid needs a owner to be appended*/
		const SWidget* m_owner;

		//todo:implement FSlateRect

		/*the size of the grid in cells*/
		FIntPoint m_num_cells;

		/*where the 0,0 of the upper-left-most cell to in desktop space*/
		glm::vec2 m_grid_origin;

		/*where the 0,0 of the upper-left-most cell corresponds to in window space*/
		glm::vec2 m_grid_window_origin;

		/*the size of the current grid*/
		glm::vec2 m_grid_size;

		/*the current slate user index that should be associated with any added widgets*/
		int32_t m_current_user_index;
	};
}
