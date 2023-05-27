#include <PreCompileHeader.h>

#include "SGridPanel.h"

#include "SlateCore/Layout/ArrangedChildren.h"//FArrangedChildren depends on it

#include "SlateCore/Types/PaintArgs.h"//FPaintArgs depends on it

#include "SlateCore/Layout/LayoutUtils.h"//AlignmentArrangeResult depends on it

namespace DoDo {
	//void SGridPanel::FSlot::Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
	//{
	//
	//}
	SLATE_IMPLEMENT_WIDGET(SGridPanel)
	void SGridPanel::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//todo:register
	}

	void SGridPanel::FSlot::Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
	{
		TBasicLayoutWidgetSlot<FSlot>::Construct(slot_owner, std::move(in_args));

		if (in_args._Column.has_value())
		{
			m_column_param = std::max(0, in_args._Column.value());
		}
		if (in_args._ColumnSpan.has_value())
		{
			m_column_span_param = std::max(1, in_args._ColumnSpan.value());
		}
		if (in_args._Row.has_value())
		{
			m_row_param = std::max(0, in_args._Row.value());
		}
		if (in_args._RowSpan.has_value())
		{
			m_row_span_param = std::max(1, in_args._RowSpan.value());
		}
		if (in_args._Layer.has_value())
		{
			m_layer_param = in_args._Layer.value();
		}
		if (in_args._Nudge.has_value())
		{
			m_nudge_param = in_args._Nudge.value();
		}
	}

	int32_t SGridPanel::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		FArrangedChildren arranged_children(EVisibility::All);
		this->Arrange_Children(allotted_geometry, arranged_children);

		//because we paint multiple children, we must track the maximum layer id that they produced in case one of our parents
		//wants to an overlay for all of its contents
		int32_t max_layer_id = layer_id;

		const FPaintArgs new_args = args.with_new_parent(this);
		const bool b_should_be_enabled = should_be_enabled(b_parent_enabled);

		//we need to iterate over slots, because slots know the grid layers, this isn't available in the arranged children
		//some slots do not show up(they are hidden/collapsed), we need a 2nd index to skip over them

		//grid layers must ensure that everything in LayerN is below LayerN + 1, in other words
		//every grid layer group must start at the current max layer id (similar to how SOverlay works)

		if (arranged_children.num())
		{
			int32_t last_grid_layer = 0;
			for (int32_t child_index = 0; child_index < arranged_children.num() && child_index < m_slots.num(); ++child_index)
			{
				FArrangedWidget& cur_widget = arranged_children[child_index];
				if (cur_widget.m_widget->get_visibility().is_visible())
				{
					const FSlot& cur_slot = m_slots[child_index];

					//todo:check widget culled
					if (last_grid_layer != cur_slot.get_layer())
					{
						//we starting a new grid layer group !
						last_grid_layer = cur_slot.get_layer();
						//ensure that everything here is drawn on top of
						layer_id = max_layer_id + 1;
					}

					const int32_t cur_widgets_max_layer_id = cur_widget.m_widget->paint(
						new_args,
						cur_widget.m_geometry,
						my_culling_rect,
						out_draw_elements,
						layer_id,
						in_widget_style,
						b_should_be_enabled
					);

					max_layer_id = std::max(max_layer_id, cur_widgets_max_layer_id);
				}
			}
		}

		return max_layer_id;
	}

	void calculate_stretched_cell_sizes(std::vector<float>& out_sizes, float allotted_size, const std::vector<float>& in_desired_sizes, const std::vector<TAttribute<float>>& coefficients)
	{
		const int32_t num_coefficients = coefficients.size();

		float coefficient_total = 0.0f;

		for (int32_t index = 0; index < in_desired_sizes.size(); ++index)
		{
			const float coefficient = index < num_coefficients ? coefficients[index].Get(0) : 0;

			//compute the total space available for stretchy columns
			if (coefficient == 0)
			{
				allotted_size -= in_desired_sizes[index];
			}
			else
			{
				//compute the denominator for dividing up the stretchy column space
				coefficient_total += coefficient;
			}
		}

		for (int32_t index = 0; index < in_desired_sizes.size(); ++index)
		{
			const float coefficient = index < num_coefficients ? coefficients[index].Get(0) : 0;

			//figure out how big each column needs to be
			out_sizes[index] = coefficient != 0
				? (coefficient / coefficient_total * allotted_size)
				: in_desired_sizes[index];
		}
	}

	void SGridPanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
		//prepare phase
		//prepare some data for arranging children
		//final columns will be populated with column sizes that include the stretched column sizes
		//then we will build partial sums so that we easily handle column spans
		//repeat the same for rows
		float column_coeff_total = 0.0f;
		std::vector<float> final_columns;
		if (m_columns.size() > 0)
		{
			final_columns.resize(m_columns.size());
			final_columns[final_columns.size() - 1] = 0.0f;
		}

		float row_coeff_total = 0.0f;
		std::vector<float> final_rows;
		if (m_rows.size() > 0)
		{
			final_rows.resize(m_rows.size());
			final_rows[final_rows.size() - 1] = 0.0f;
		}

		calculate_stretched_cell_sizes(final_columns, allotted_geometry.get_local_size().x, m_columns, m_col_fill_coefficients);
		calculate_stretched_cell_sizes(final_rows, allotted_geometry.get_local_size().y, m_rows, m_row_fill_coefficients);

		//build up partial sums for row and column sizes so that we can handle column and row spans coveniently
		compute_partial_sums(final_columns);
		compute_partial_sums(final_rows);

		//arrange phase
		for (int32_t slot_index = 0; slot_index < m_slots.num(); ++slot_index)
		{
			const FSlot& cur_slot = m_slots[slot_index];
			const EVisibility child_visibility = cur_slot.get_widget()->get_visibility();

			if (arranged_children.Accepts(child_visibility))
			{
				//figure out the position of this cell
				const glm::vec2 this_cell_offset(final_columns[cur_slot.get_column()], final_rows[cur_slot.get_row()]);

				//figure out the size of this slot, takes row span into account
				//we use the properties of partial sums arrays to achieve this
				//row / column span may be an arbitrarily large value above 1, so clamp it to the array bounds
				const int32_t last_column_idx = std::min(cur_slot.get_column() + cur_slot.get_column_span(), (int32_t)final_columns.size() - 1);
				const int32_t last_row_idx = std::min(cur_slot.get_row() + cur_slot.get_row_span(), (int32_t)final_rows.size() - 1);
				const glm::vec2 cell_size(
					final_columns[last_column_idx] - this_cell_offset.x,
					final_rows[last_row_idx] - this_cell_offset.y
				);

				//do the standard arrangement of elements within a slot
				//takes care of alignment and padding
				const FMargin slot_padding(cur_slot.get_padding());
				AlignmentArrangeResult x_axis_result = Align_Child<Orient_Horizontal>(cell_size.x, cur_slot, slot_padding);
				AlignmentArrangeResult y_axis_result = Align_Child<Orient_Vertical>(cell_size.y, cur_slot, slot_padding);

				//output the result
				arranged_children.add_widget(child_visibility, allotted_geometry.make_child(
					cur_slot.get_widget(),
					this_cell_offset + glm::vec2(x_axis_result.m_offset, y_axis_result.m_offset) + cur_slot.get_nudge(),
					glm::vec2(x_axis_result.m_size, y_axis_result.m_size)
				));
			}
		}
	}
	void SGridPanel::cache_desired_size(float in_layout_scale_multiplier)
	{
		compute_desired_cell_sizes(m_columns, m_rows);

		m_total_desired_sizes = glm::vec2(0.0f);

		for (int32_t col_id = 0; col_id < m_columns.size(); ++col_id)
		{
			m_total_desired_sizes.x += m_columns[col_id];
		}

		for (int32_t row_id = 0; row_id < m_rows.size(); ++row_id)
		{
			m_total_desired_sizes.y += m_rows[row_id];
		}

		SPanel::cache_desired_size(in_layout_scale_multiplier);
	}
	glm::vec2 SGridPanel::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		return m_total_desired_sizes;
	}
	FChildren* SGridPanel::Get_Children()
	{
		return &m_slots;
	}
	void SGridPanel::compute_desired_cell_sizes(std::vector<float>& out_columns, std::vector<float>& out_rows) const
	{
		memset(out_columns.data(), 0, out_columns.size() * sizeof(float));
		memset(out_rows.data(), 0, out_rows.size() * sizeof(float));

		for (int32_t slot_index = 0; slot_index < m_slots.num(); ++slot_index)
		{
			const FSlot& cur_slot = m_slots[slot_index];

			if (cur_slot.get_widget()->get_visibility() != EVisibility::Collapsed)
			{
				//the slots wants to be as big as its content along with the required padding
				const glm::vec2 slot_desired_size = cur_slot.get_widget()->get_desired_size() + cur_slot.get_padding().get_desired_size();

				//if the slot has a (colspan, rowspan) of (1, 1) it will only affect that slot
				//for larger spans, the slots size will be evenly distributed across all the affected slots
				//for spans that are past the end of the slot array, only distribute to the size to existing columns, eg
				//| [0] | [1] | [2] |
				//| S=1 | Span = 5  | <- size is only distributed between columns 1 and 2, until more slots exits
				const int32_t max_columns = std::min(cur_slot.get_column() + cur_slot.get_column_span(), (int32_t)out_columns.size());
				const int32_t max_rows = std::min(cur_slot.get_row() + cur_slot.get_row_span(), (int32_t)out_rows.size());

				const glm::vec2 size_contribution(slot_desired_size.x / (max_columns - cur_slot.get_column()), slot_desired_size.y / (max_rows - cur_slot.get_row()));

				//distribute the size contributions over all the columns and rows that this slot spans
				distribute_size_contributions(size_contribution.x, out_columns, cur_slot.get_column(), max_columns);
				distribute_size_contributions(size_contribution.y, out_rows, cur_slot.get_row(), max_rows);
			}
		}
	}
	void SGridPanel::notify_slot_changed(const FSlot* in_slot, bool b_slot_layer_changed)
	{
		//keep the size of the grid up to date
		//we need an extra cell at the end for easily figuring out the size across any number of cells
		//by doing columns[end] - columns[start] or rows[end] - row[start]
		//the first columns[]/rows[] entry will be 0
		//for a grid with 2 rows (index 0 and 1), we need 3 slots, thus the + 2 here
		const int32_t num_columns_required_for_this_slot = in_slot->get_column() + 2;
		if (num_columns_required_for_this_slot > m_columns.size())
		{
			int32_t populate_number = num_columns_required_for_this_slot - m_columns.size();
			for (int32_t i = 0; i < populate_number; ++i)
			{
				m_columns.push_back(0);
			}
		}

		const int32_t num_rows_required_for_this_slot = in_slot->get_row() + 2;
		if (num_rows_required_for_this_slot > m_rows.size())
		{
			int32_t populate_number = num_rows_required_for_this_slot - m_rows.size();
			for (int32_t i = 0; i < populate_number; ++i)
			{
				m_rows.push_back(0);
			}
		}
	}
	void SGridPanel::compute_partial_sums(std::vector<float>& turn_me_into_partial_sums) //prefix sum
	{
		//we assume there is a 0-valued item already at the end of this array
		//we need it so that we can compute the original values
		//by doing array[N] - arrag[N - 1]

		float last_value = 0;
		float sum_so_far = 0;
		for (int32_t index = 0; index < turn_me_into_partial_sums.size(); ++index)
		{
			last_value = turn_me_into_partial_sums[index];
			turn_me_into_partial_sums[index] = sum_so_far;
			sum_so_far += last_value;
		}
	}
	void SGridPanel::distribute_size_contributions(float size_contribution, std::vector<float>& distribute_over_me, int32_t start_index, int32_t upper_bound)
	{
		for (int32_t index = start_index; index < upper_bound; ++index)
		{
			//each column or row only needs to get bigger if its current size does not already accommodate it
			distribute_over_me[index] = std::max(size_contribution, distribute_over_me[index]);
		}
	}
}