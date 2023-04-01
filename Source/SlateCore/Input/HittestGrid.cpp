#include <PreCompileHeader.h>

#include "HittestGird.h"
#include "SlateCore/Layout/Geometry.h"
#include "SlateCore/Widgets/SWidget.h"

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
		//const glm::vec2 cursor_position_in_grid = desktop_space_coordinate - m_grid_origin;
		const glm::vec2 cursor_position_in_grid = desktop_space_coordinate - m_grid_origin;//note:this desktop_space_coordinate is local cursor position(interms of glfw)

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

				if(first_hit_widget)
				{
					//todo:construct a widget path from current closest widget to swindow

					std::vector<FWidgetAndPointer> path;

					std::shared_ptr<SWidget> current_widget = first_hit_widget;

					while(current_widget) //note:construct a widget path from current widget to the root widget
					{
						FGeometry desktop_space_geometry = current_widget->get_paint_space_geometry();

						//todo:implement desktop space geometry
						desktop_space_geometry.append_transform(FSlateLayoutTransform(m_grid_origin - m_grid_window_origin));

						path.emplace_back(FArrangedWidget(current_widget, desktop_space_geometry));

						current_widget = current_widget->advanced_get_paint_parent_widget();
					}

					std::reverse(path.begin(), path.end());//SWindow is first

					return path;
				}
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

	static glm::vec2 closest_point_on_segment_2d(const glm::vec2& point, const glm::vec2& start_point, const glm::vec2& end_point)
	{
		const glm::vec2 segment = end_point - start_point;
		const glm::vec2 vect_to_point = point - start_point;

		//see if closest point is before start point
		const float dot1 = end_point.x * vect_to_point.x + end_point.y * vect_to_point.y;
		if(dot1 <= 0)
		{
			return start_point;
		}

		//see if closest point is beyond end point
		const float dot2 = segment.x * segment.x + segment.y * segment.y;
		if(dot2 <= dot1)
		{
			return end_point;
		}

		//closest point is within segment
		return start_point + segment * (dot1 / dot2);//note:need to understand
	}

	glm::vec2 closest_point_on_slate_rotated_rect(const glm::vec2& point, const FSlateRotatedRect& rotated_rect)
	{
		//no need to do any testing if we are inside of the rect
		if(rotated_rect.is_under_location(point))
		{
			return point;
		}

		const static int32_t num_of_corners = 4;
		glm::vec2 corners[num_of_corners];
		corners[0] = rotated_rect.m_top_left;
		corners[1] = corners[0] + rotated_rect.m_extent_x;
		corners[2] = corners[1] + rotated_rect.m_extent_y;
		corners[3] = corners[2] + rotated_rect.m_extent_y;

		glm::vec2 ret_point;
		float closest_dist_sq = FLT_MAX;
		for(int32_t i = 0; i < num_of_corners; ++i)
		{
			//grab the closest point along the line segment
			const glm::vec2 closest_point = closest_point_on_segment_2d(point, corners[i], corners[(i + 1) % num_of_corners]);

			//get the distance between the two
			const float test_dist = (point.x - closest_point.x) * (point.x - closest_point.x) + (point.y - closest_point.y) * (point.y - closest_point.y);

			//if the distance is smaller than the current smallest, update our closest
			if(test_dist < closest_dist_sq)
			{
				ret_point = closest_point;
				closest_dist_sq = test_dist;
			}
		}

		return ret_point;//todo:implement this function
	}

	float dist_squared(const glm::vec2& lhs, const glm::vec2& rhs)
	{
		return (lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y);
	}

	float distance_sq_to_slate_rotated_rect(const glm::vec2& point, const FSlateRotatedRect& rotated_rect)
	{
		return dist_squared(closest_point_on_slate_rotated_rect(point, rotated_rect), point);
	}

	bool is_overlapping_slate_rotated_rect(const glm::vec2& point, const float radius, const FSlateRotatedRect& rotated_rect)
	{
		return distance_sq_to_slate_rotated_rect(point, rotated_rect) <= (radius * radius);//what it is?
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
		//return m_grid->m_widget_array[m_widget_index];
		auto it = m_grid->m_widget_array.find(m_widget_index);

		//note:fix me, will be empty
		return it->second;
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

			//interms of layer id to sort
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

			//note:get the (x, y) FCell's widgets, and interms of layer id to sort
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

					const FGeometry test_geometry = test_widget->get_paint_space_geometry();//todo:implement get paint space geometry

					bool b_point_inside_clip_masks = true;

					if(b_point_inside_clip_masks)
					{
						//compute the render space clipping rect
						//todo:implement this
						const FSlateRotatedRect window_oriented_clip_rect = transform_rect(
							concatenate(
								inverse(test_geometry.get_accumulated_layout_transform()),
								test_geometry.get_accumulated_render_transform()),
								FSlateRotatedRect(test_geometry.get_layout_bounding_rect()
							)
						);//todo:what it is?


						//note:check cursor position in clip rect
						if(is_overlapping_slate_rotated_rect(window_space_coordinate, params.m_radius, window_oriented_clip_rect))
						{
							//todo:implement Is Overlapping Slate Rotated Rect
							const bool b_needs_distance_search = params.m_radius > 0.0f;
							const float dis_sq = b_needs_distance_search ? distance_sq_to_slate_rotated_rect(window_space_coordinate, window_oriented_clip_rect) : 0.0f;
							return FIndexAndDistance(widget_indexes[i], dis_sq);
						}
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

	void FHittestGrid::clear_internal(int32_t total_cells)
	{
		m_cells.clear();//clear

		for (size_t i = 0; i < total_cells; ++i)
		{
			m_cells.push_back(FCell());
		}

		m_widget_map.clear();
		m_widget_array.clear();
		m_append_grid_array.clear();
	}

	void FHittestGrid::FCell::add_index(int32_t widget_index)
	{
		m_widget_indexes.push_back(widget_index);
	}

	void FHittestGrid::FCell::remove_index(int32_t widget_index)
	{

		for(auto it = m_widget_indexes.begin(); it != m_widget_indexes.end(); ++it)
		{
			if(*it == widget_index)
			{
				//to erase this
				m_widget_indexes.erase(it);
				break;
			}
		}
	}

	bool FHittestGrid::is_valid_cell_coord(const FIntPoint& cell_coord) const
	{
		return is_valid_cell_coord(cell_coord.x, cell_coord.y);
	}

	bool FHittestGrid::Set_Hittest_Area(const glm::vec2& hittest_position_in_desktop,
		const glm::vec2& hittest_dimensions, const glm::vec2& hittest_offset_in_window)
	{
		//clear hittest grid area
		bool b_was_cleared = false;

		//if the size of the hit test area changes we need to clear it out
		if(m_grid_size != hittest_dimensions)
		{
			m_grid_size = hittest_dimensions;
			m_num_cells = FIntPoint((int32_t)(m_grid_size.x / cell_size.x), (int32_t)(m_grid_size.y / cell_size.y));

			const int32_t new_total_cells = m_num_cells.x * m_num_cells.y;

			clear_internal(new_total_cells);

			b_was_cleared = true;
		}

		m_grid_origin = hittest_position_in_desktop;
		m_grid_window_origin = hittest_offset_in_window;

		return b_was_cleared;
	}

	void FHittestGrid::clear()
	{
		const int32_t total_cells = m_cells.size();
		clear_internal(total_cells);
	}

	void FHittestGrid::add_widget(const SWidget* in_widget, int32_t in_batch_priority_group, int32_t layer_id)
	{
		if (!in_widget->get_visibility().is_hit_test_visible())
		{
			return;
		}
		//track the widget and identify it's widget index
		FGeometry grid_space_geometry = in_widget->get_paint_space_geometry();
		grid_space_geometry.append_transform(FSlateLayoutTransform(-m_grid_window_origin));

		//currently using grid offset because the grid covers all desktop space
		const FSlateRect bounding_rect = grid_space_geometry.get_render_bounding_rect();

		//starting and ending cells covered by this widget
		const FIntPoint upper_left_cell = get_cell_coordinate(bounding_rect.get_top_left());
		const FIntPoint lower_right_cell = get_cell_coordinate(bounding_rect.get_bottom_right());

		const int64_t primary_sort = (((int64_t)in_batch_priority_group << 32) | layer_id);

		bool b_add_widget = true;
		auto it = m_widget_map.find(in_widget);
		if(it != m_widget_map.end())
		{
			FWidgetData& widget_data = m_widget_array[it->second];

			if(widget_data.m_upper_left_cell != upper_left_cell || widget_data.m_lower_right_cell != lower_right_cell)
			{
				remove_widget(in_widget);
			}
			else
			{
				//only update
				b_add_widget = false;
				widget_data.m_primary_sort = primary_sort;
				widget_data.m_user_index = m_current_user_index;
			}
		}

		//add index
		if(b_add_widget)
		{
			m_widget_map.insert({in_widget, 0});

			auto it = m_widget_map.find(in_widget);

			//int32_t& widget_index = ret.second;
			int32_t& widget_index = it->second;

			//find a first free index, to construct a element in this map
			int32_t free_index = -1;
			for(int32_t i = 0; i < m_widget_array.size(); ++i)
			{
				if(m_widget_array.find(i) == m_widget_array.end())
				{
					free_index = i;
					break;
				}
			}

			if (free_index == -1) free_index = m_widget_array.size();

			widget_index = free_index;

			//m_widget_array.insert(std::make_pair<int32_t, FWidgetData>(free_index, FWidgetData(const_cast<SWidget*>(in_widget)->shared_from_this(), upper_left_cell, lower_right_cell, primary_sort, m_current_user_index)));

			m_widget_array[free_index] = FWidgetData(const_cast<SWidget*>(in_widget)->shared_from_this(), upper_left_cell, lower_right_cell, primary_sort, m_current_user_index);

			//m_widget_array.emplace_back(const_cast<SWidget*>(in_widget)->shared_from_this(), upper_left_cell, lower_right_cell, primary_sort, m_current_user_index);
			//
			//widget_index = m_widget_array.size() - 1;

			for(int32_t x_index = upper_left_cell.x; x_index <= lower_right_cell.x; ++x_index)
			{
				for(int32_t y_index = upper_left_cell.y; y_index <= lower_right_cell.y; ++y_index)
				{
					if(is_valid_cell_coord(x_index, y_index))
					{
						cell_at(x_index, y_index).add_index(widget_index);//add widget index, to pointer widget array
					}
				}
			}
		}
	}

	void FHittestGrid::remove_widget(const SWidget* in_widget)
	{
		int32_t widget_index = -1;

		auto it = m_widget_map.find(in_widget);

		//widget_index = it->second;

		if(it != m_widget_map.end())
		{
			widget_index = it->second;

			const FWidgetData& widget_data = m_widget_array[widget_index];

			//starting and ending cells covered by this widget
			const FIntPoint& upper_left_cell = widget_data.m_upper_left_cell;
			const FIntPoint& lower_right_cell = widget_data.m_lower_right_cell;

			for(int32_t x_index = upper_left_cell.x; x_index <= lower_right_cell.x; ++x_index)
			{
				for(int32_t y_index = upper_left_cell.y; y_index <= lower_right_cell.y; ++y_index)
				{
					cell_at(x_index, y_index).remove_index(widget_index);
				}
			}

			//m_widget_array.remove_at
			m_widget_array.erase(widget_index);//delete this

			m_widget_map.erase(it);
		}

		//remove_grid(in_widget);
	}
}
