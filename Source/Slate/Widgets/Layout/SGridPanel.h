#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"//TBasicLayoutWidgetSlot depends on it

#include "SlateCore/Layout/Children.h"//TPanelChildren

namespace DoDo {
	class SGridPanel : public SPanel
	{
		SLATE_DECLARE_WIDGET(SGridPanel, SPanel)

	public:
		//used by the mandatory named parameter in FSlot
		class Layer
		{
		public:
			explicit Layer(int32_t in_layer)
				: m_the_layer(in_layer)
			{}
			int32_t m_the_layer;
		};

		class FSlot : public TBasicLayoutWidgetSlot<FSlot>
		{
			friend SGridPanel;

		public:
			/*default values for a slot*/
			FSlot(int32_t column, int32_t row, int32_t in_layer)
				: TBasicLayoutWidgetSlot<FSlot>(HAlign_Fill, VAlign_Fill)
				, m_column_param(column)
				, m_column_span_param(1)
				, m_row_param(row)
				, m_row_span_param(1)
				, m_layer_param(in_layer)
				, m_nudge_param(glm::vec2(0.0f))
			{}

			SLATE_SLOT_BEGIN_ARGS(FSlot, TBasicLayoutWidgetSlot<FSlot>)
				/*which column in the grid this cell belongs to*/
				SLATE_ARGUMENT(std::optional<int32_t>, Column)
				/*how many columns this slot spans over*/
				SLATE_ARGUMENT(std::optional<int32_t>, ColumnSpan)
				/*which row in the grid this cell belongs to*/
				SLATE_ARGUMENT(std::optional<int32_t>, Row)
				/*how many rows this slot spans over*/
				SLATE_ARGUMENT(std::optional<int32_t>, RowSpan)
				/*positive values offset this cell to be hit-tested and drawn on top others, default is 0, i.e. no offset*/
				SLATE_ARGUMENT(std::optional<int32_t>, Layer)
				/*offset this slot's content by some amount, positive values offset to lower right*/
				SLATE_ARGUMENT(std::optional<glm::vec2>, Nudge)
			SLATE_SLOT_END_ARGS()

			void Construct(const FChildren& slot_owner, FSlotArguments&& in_args);

			/*which column in the grid this cell belongs to*/
			int32_t get_column() const
			{
				return m_column_param;
			}

			void set_column(int32_t column)
			{
				column = std::max(0, column);

				if (column != m_column_param)
				{
					m_column_param = column;
					notify_slot_changed();
				}
			}

			/*how many columns this slot spans over*/
			int32_t get_column_span() const
			{
				return m_column_span_param;
			}

			void set_column_span(int32_t column_span)
			{
				column_span = std::max(1, column_span);
				if (column_span != m_column_span_param)
				{
					m_column_span_param = column_span;
					notify_slot_changed();
				}
			}

			/*which row in the grid this cell belongs to*/
			int32_t get_row() const
			{
				return m_row_param;
			}

			void set_row(int32_t row)
			{
				row = std::max(0, row);
				if (row != m_row_param)
				{
					m_row_param = row;
					notify_slot_changed();
				}
			}

			/*how many rows this slot spans over*/
			int32_t get_row_span() const
			{
				return m_row_span_param;
			}

			void set_row_span(int32_t row_span)
			{
				row_span = std::max(1, row_span);
				if (row_span != m_row_span_param)
				{
					m_row_span_param = row_span;
					notify_slot_changed();
				}
			}

			/*positive values offset this cell to be hit-tested and drawn on top of others, default is 0; i.e. no offset*/
			int32_t get_layer() const
			{
				return m_layer_param;
			}

			void set_layer(int32_t layer)
			{
				if (layer != m_layer_param)
				{
					m_layer_param = layer;
					const bool b_slot_layer_changed = true;
					notify_slot_changed(b_slot_layer_changed);
				}
			}

			/*offset this slot's content by some amount, positive values offset to lower right*/
			glm::vec2 get_nudge() const
			{
				return m_nudge_param;
			}

			void set_nudge(const glm::vec2& nudge)
			{
				m_nudge_param = nudge;
				//todo:add invalidate
			}
		private:
			/*the panel that contains this slot*/
			std::weak_ptr<SGridPanel> m_panel;

			int32_t m_column_param;
			int32_t m_column_span_param;
			int32_t m_row_param;
			int32_t m_row_span_param;
			int32_t m_layer_param;
			glm::vec2 m_nudge_param;

			/*notify that the slot was changed*/
			void notify_slot_changed(bool b_slot_layer_changed = false)
			{
				if (m_panel.lock())
				{
					m_panel.lock()->notify_slot_changed(this, b_slot_layer_changed);
				}
			}
		};

		SLATE_BEGIN_ARGS(SGridPanel)
			{
				_Visibility = EVisibility::SelfHitTestInvisible;
			}

			SLATE_SLOT_ARGUMENT(FSlot, Slots)

			/*specify a column to stretch instead of sizing to content*/
			FArguments& fill_column(int32_t column_id, const TAttribute<float>& coefficient)
			{
				while (m_col_fill_coefficients.size() <= column_id)
				{
					m_col_fill_coefficients.push_back(0);
				}
				m_col_fill_coefficients[column_id] = coefficient;
				return Me();
			}

			/*specify a row to stretch instead of sizing to content*/
			FArguments& fill_row(int32_t row_id, const TAttribute<float>& coefficient)
			{
				while (m_row_fill_coefficients.size() <= row_id)
				{
					m_row_fill_coefficients.push_back(0);
				}
				m_row_fill_coefficients[row_id] = coefficient;
				return Me();
			}
			/*coefficients for columns that need to stretch instead of size to content*/
			std::vector<TAttribute<float>> m_col_fill_coefficients;

			/*coefficients for rows that need to stretch instead of size to content*/
			std::vector<TAttribute<float>> m_row_fill_coefficients;
		SLATE_END_ARGS()

		SGridPanel();

		void Construct(const FArguments& in_args);

		/*
		* used by declarative syntax to create a slot in the specified column, row and layer
		*/
		static FSlot::FSlotArguments Slot(int32_t column, int32_t row, Layer in_layer = Layer(0));

	public:
		//swidget interface
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual void cache_desired_size(float in_layout_scale_multiplier) override;

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;

		virtual FChildren* Get_Children() override;

		/*
		* find the index where the given slot should be inserted into the list of slots based on its layer param, such that slots are sorted by layer
		* 
		* @param the newly-allocated slot to insert
		* @return the index where the slot shoul be inserted
		*/
		int32_t find_insert_slot_location(const FSlot* in_slot);

		/*compute the sizes of columns and rows needed to fit all the slots in this grid*/
		void compute_desired_cell_sizes(std::vector<float>& out_columns, std::vector<float>& out_rows) const;

		/*
		* callback used to resize our internal arrays if a slot (or slot span) is changed
		* 
		* @param InSlot the slot that has just changed
		* @param bSlotLayerChanged whether the slot layer changed
		*/
		void notify_slot_changed(const FSlot* in_slot, bool b_slot_layer_changed = false);

	private:
		/*
		* given an array of values, re-populate the array such that every contains the partial sums up to that element
		* i.e. array[N] = Array.Sum(0 .. N - 1)
		* 
		* the resulting array is 1-element longer
		*/
		static void compute_partial_sums(std::vector<float>& turn_me_into_partial_sums);

		/*given a size contribution, distribute it to the elements in distribute over me at indexes from [start index .. upper bound)*/
		static void distribute_size_contributions(float size_contribution, std::vector<float>& distribute_over_me, int32_t start_index, int32_t upper_bound);
		/*the slots that are place into various grid locations*/
		TPanelChildren<FSlot> m_slots;

		/*offsets of each column from the beginning of the grid
		* includes a faux value at the end of the array for finding the size of the last cell
		*/
		std::vector<float> m_columns;

		/*offsets of each row from the beginning of the grid
		* includes a faux value at the end of the array for finding the size of the last cell
 		*/
		std::vector<float> m_rows;

		/*total desires size along each axis*/
		glm::vec2 m_total_desired_sizes;

		/*fill coefficients for the columns*/
		std::vector<TAttribute<float>> m_col_fill_coefficients;

		/*fill coefficients for the rows*/
		std::vector<TAttribute<float>> m_row_fill_coefficients;
	};
}