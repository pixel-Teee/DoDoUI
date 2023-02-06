#include <PreCompileHeader.h>

#include "HittestGird.h"
#include "SlateCore/Layout/Geometry.h"

namespace DoDo
{
	FHittestGrid::FHittestGrid()
		: m_widget_map()
		, m_widget_array()
		, m_cells()
		, m_append_grid_array()
		, m_owner(nullptr)
		, m_num_cells(0, 0)
		, m_grid_origin(0, 0)
		, m_grid_size(0, 0)
		, m_current_user_index(-1)
	{
	}

	//float distance_sq_to_slate_

	const glm::vec2 cell_size(128.0f, 128.0f);

	struct FHittestGrid::FGridTestingParams//private class
	{
		FGridTestingParams()
			: m_cell_coord(-1, -1)
			, m_cursor_position_in_grid(glm::vec2(0.0f, 0.0f))
			, m_radius(-1.0f)
			, m_b_test_widget_is_interative(false)
		{}

		FIntPoint m_cell_coord;
		glm::vec2 m_cursor_position_in_grid;
		float m_radius;
		bool m_b_test_widget_is_interative;
	};

	std::vector<FWidgetAndPointer> FHittestGrid::get_bubble_path(glm::vec2 desktop_space_coordinate,
		float cursor_radius, bool b_ignore_enabled_status, int32_t user_index)
	{
		const glm::vec2 cursor_position_in_grid = desktop_space_coordinate - m_grid_origin;

		if(m_widget_array.size() > 0 && m_cells.size() > 0)
		{
			FGridTestingParams testing_params;
			testing_params.m_cursor_position_in_grid = cursor_position_in_grid;
			testing_params.m_cell_coord = get_cell_coordinate(cursor_position_in_grid);
			testing_params.m_radius = 0.0f;
			testing_params.m_b_test_widget_is_interative = false;

			//first add the exact point test results
			const FIndexAndDistance best_hit = get_hit_index_from_cell_index(testing_params);

			if(best_hit.is_valid())
			{
				const FWidgetData& best_hit_widget_data = best_hit.get_widget_data();

				const std::shared_ptr<SWidget> first_hit_widget = best_hit_widget_data.get_widget();


			}
		}

		return std::vector<FWidgetAndPointer>();
	}

	FIntPoint FHittestGrid::get_cell_coordinate(glm::vec2 position) const
	{
		int32_t x_index = (int32_t)(position.x / cell_size.x);
		int32_t y_index = (int32_t)(position.y / cell_size.y);

		return FIntPoint(std::min(std::max(x_index, 0), m_num_cells.x - 1),
			std::min(std::max(y_index, 0), m_num_cells.y - 1));
	}

	void FHittestGrid::get_collapsed_hittest_grid(FCollapsedHittestGridArray& out_result) const
	{
		out_result.push_back(this);//push this

		//for(const FAppendGridData& appended_grid_data : m_append_grid_array)
		//{
		//	if(const std::shared_ptr<const FHittestGrid>& appended_grid = appended_grid_data.m_grid.lock())
		//	{
		//		//todo:check for recursion
		//	}
		//}
	}

	const FHittestGrid::FWidgetData& FHittestGrid::FWidgetIndex::get_widget_data() const
	{
		return m_grid->m_widget_array[m_widget_index];
	}

	void FHittestGrid::get_collapsed_widgets(FCollapsedWidgetsArray& out, const int32_t x, const int32_t y) const
	{
		const int32_t cell_index = y * m_num_cells.x + x;

		//todo:check cell index

		FCollapsedHittestGridArray all_hit_test_grids;
		get_collapsed_hittest_grid(all_hit_test_grids);

		for(const FHittestGrid* hittest_grid : all_hit_test_grids)
		{
			const std::vector<int32_t>& widgets_indexes = hittest_grid->cell_at(x, y).get_widget_indexes();

			for(int32_t widget_index : widgets_indexes)
			{
				out.emplace_back(hittest_grid, widget_index);
			}
		}

		std::stable_sort(out.begin(), out.end(), [](const FWidgetIndex& a, const FWidgetIndex& b)
		{
			const FWidgetData& widget_data_a = a.get_widget_data();
			const FWidgetData& widget_data_b = b.get_widget_data();

			return widget_data_a.m_primary_sort < widget_data_b.m_primary_sort;//todo:implement secondary sort
		});
	}


	inline FHittestGrid::FIndexAndDistance FHittestGrid::get_hit_index_from_cell_index(
		const FGridTestingParams& params) const
	{
		if (is_valid_cell_coord(params.m_cell_coord))
		{
			//get the cell and sort it
			FCollapsedWidgetsArray widget_indexes;
			get_collapsed_widgets(widget_indexes, params.m_cell_coord.x, params.m_cell_coord.y);

			//consider front-most widgets first for hittesting
			for(int32_t i = widget_indexes.size() - 1; i >= 0; --i)
			{
				const FWidgetData& test_candidate = widget_indexes[i].get_widget_data();
				const std::shared_ptr<SWidget> test_widget = test_candidate.get_widget();

				//when performing a point hittest, accept all hittestable widgets
				//when performing a hittest with a radius, only grab interactive widgets
				const bool b_is_valid_widget = test_widget && (!params.m_b_test_widget_is_interative);

				if(b_is_valid_widget)
				{
					const glm::vec2 window_space_coordinate = params.m_cursor_position_in_grid + m_grid_window_origin;

					const FGeometry& test_geometry;//todo:implement get paint space geometry

					bool b_point_inside_clip_masks = true;

					if(b_point_inside_clip_masks)
					{
						//compute the render space clipping rect
						//todo:implement this

						const bool b_needs_distance_search = params.m_radius > 0.0f;
						const float dis_sq = 0.0f;//todo:implement this
							return FIndexAndDistance(widget_indexes[i], dis_sq);
					}
				}
			}
		}

		return FIndexAndDistance();
	}

	bool FHittestGrid::is_valid_cell_coord(const int32_t x_coord, const int32_t y_coord) const
	{
		return x_coord >= 0 && x_coord < m_num_cells.x&& y_coord >= 0 && y_coord < m_num_cells.y;
	}

	bool FHittestGrid::is_valid_cell_coord(const FIntPoint& cell_coord) const
	{
		return is_valid_cell_coord(cell_coord.x, cell_coord.y);
	}
}
