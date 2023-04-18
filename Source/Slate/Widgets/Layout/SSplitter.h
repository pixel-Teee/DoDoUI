#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/SlotBase.h"//FSlot depends on it
#include "SlateCore/Layout/Children.h"//TPanelChildren
#include "SlateCore/Layout/LayoutGeometry.h"//FLayoutGeometry depends on it
#include "SlateCore/Styling/CoreStyle.h"
#include "SlateCore/Styling/ISlateStyle.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "SlateCore/Styling/SlateTypes.h"//FSplitterStyle depends on it

namespace DoDo {
	namespace ESplitterResizeMode
	{
		enum Type
		{
			/*resize the selected slot, if space is needed, then resize the next resizable slot*/
			FixedPosition,
			/*resize the selected slot, if space is needed, then resize the last resizable slot*/
			FixedSize,
			/*resize the selected slot by redistributing the available space with the following resizable slots*/
			Fill
		};
	}
	struct FSplitterStyle;
	/*
	* SSplitter divides its allotted area into N segments, where N is the number of children it has
	* it allows the users to resize the children along the splitters axis, that is, horizontally or vertically
	*/
	class SSplitter : public SPanel//todo:add input and output
	{
	public:
		/*how should a child's size be determined*/
		enum ESizeRule
		{
			/*get the desired size() of the content*/
			SizeToContent,

			/*use a fraction of the parent's size*/
			FractionOfParent
		};

		//using FOnSlotResized = Delegate<void(float)>;//the new size coefficient of the slot

		DECLARE_DELEGATE_OneParam(FOnSlotResized, float);

		DECLARE_DELEGATE_RetVal_OneParam(glm::vec2, FOnGetMaxSlotSize, int32_t)

		//using FOnGetMaxSlotSize = Delegate<glm::vec2(int32_t)>;

	public:

		class FSlot : public TSlotBase<FSlot>
		{
		public:
			FSlot()
				: TSlotBase<FSlot>()
				, m_sizing_rule(FractionOfParent)
				, m_size_value(1)
				, m_min_size_value(0)
			{

			}

			SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
				/*the size rule used by the slot*/
				SLATE_ATTRIBUTE(ESizeRule, SizeRule)
				/*when the rule size is set to FractionOfParent, the size of the slot is the value percentage of it's parent size*/
				SLATE_ATTRIBUTE(float, Value)
				/*minimum slot size when resizing*/
				SLATE_ATTRIBUTE(float, MinSize)
				/*can the slot be resize by the user*/
				SLATE_ARGUMENT(std::optional<bool>, Resizable)
				/*callback when the slot is resized*/
				SLATE_EVENT(FOnSlotResized, OnSlotResized)
			SLATE_SLOT_END_ARGS()

			void Construct(const FChildren& slot_owner, FSlotArguments&& in_args);

			/*when the rule size is set to fraction of parent, the size of the slot is the value percentage of its parent size*/
			void set_size_value(TAttribute<float> in_value)
			{
				m_size_value = std::move(in_value);
			}

			/*
			* can the slot be resize by the user
			* @see can be resized()
			*/
			void set_resizable(bool b_in_is_resizable)
			{
				m_b_is_resizable = b_in_is_resizable;
			}

			/*the size rule used by the slot*/
			void set_sizing_rule(TAttribute<ESizeRule> in_size_rule)
			{
				m_sizing_rule = std::move(in_size_rule);
			}

			float get_min_size() const
			{
				return m_min_size_value.Get(0.0f);
			}

			/*
			* callback when the slot is resized
			* @see can be resized()
			*/
			FOnSlotResized& on_slot_resized()
			{
				return m_on_slot_resized_handler;
			}

		public:
			/*a slot can be resize if bIsResizable and the SizeRule is a FractionOfParent or the OnSlotResized delegate is set*/
			bool can_be_resized() const;

		public:
			TAttribute<ESizeRule> m_sizing_rule;

			TAttribute<float> m_size_value;

			TAttribute<float> m_min_size_value;

			FOnSlotResized m_on_slot_resized_handler;

			std::optional<bool> m_b_is_resizable;
		};

		/*@return add a new FSlot()*/
		static FSlot::FSlotArguments Slot();

		//using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;//todo:implement this

		/*
		* add a slot to the splitter at the specified index
		* sample usage:
		*	SomeSplitter->AddSlot()
		*	[
		*		SNew(SSomeWidget)
		*	]
		* @return the new slot
		*/
		using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

		FScopedWidgetSlotArguments add_slot(int32_t at_index = -1);

		SLATE_BEGIN_ARGS(SSplitter)
			: _Style(&FCoreStyle::get().get_widget_style<FSplitterStyle>("Splitter"))
			, _Orientation(Orient_Horizontal)
			, _ResizeMode(ESplitterResizeMode::FixedPosition)
			, _PhysicalSplitterHandleSize(5.0f)
			, _HitDetectionSplitterHandleSize(5.0f)
			, _MinimumSlotHeight(20.0f)
			{}
			SLATE_SLOT_ARGUMENT(FSlot, Slots)

			/*style used to draw this splitter*/
			SLATE_STYLE_ARGUMENT(FSplitterStyle, Style)

			SLATE_ARGUMENT(EOrientation, Orientation)

			SLATE_ARGUMENT(ESplitterResizeMode::Type, ResizeMode)

			SLATE_ARGUMENT(float, PhysicalSplitterHandleSize)

			SLATE_ARGUMENT(float, HitDetectionSplitterHandleSize)

			SLATE_ARGUMENT(float, MinimumSlotHeight)

			SLATE_ATTRIBUTE(int32_t, HighlightedHandleIndex)

			//SLATE_EVENT(FOnHandleHovered, OnHandleHovered)


		SLATE_END_ARGS()

		SSplitter();

		/*
		 * Construct this widget
		 *
		 * @param InArgs the declaration data for this widget
		 */
		void Construct(const FArguments& in_args);

		/*
		* change the orientation of the splitter
		* 
		* @param NewOrientation should the splitter be horizontal or vertical
		*/
		void set_orientation(EOrientation new_orientation);

		/*
		* return the current orientation of the splitter
		*/
		EOrientation get_orientation() const;

		/*
		* get the slot at the specified index
		* 
		* @param slot index replace the child at this index
		* 
		* @return slot at the index specified by slot index
		*/
		SSplitter::FSlot& slot_at(int32_t slot_index);

		/*
		* remove the child at index to remove
		* 
		* @param IndexToRemove remove the slot and child at this index
		*/
		void remove_at(int32_t index_to_remove);

	public:
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual FReply On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual FReply On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual FCursorReply On_Cursor_Query(const FGeometry& my_geometry, const FPointerEvent& cursor_event) const override;
			 
	private:
		std::vector<FLayoutGeometry> arrange_children_for_layout(const FGeometry& allotted_geometry) const;
	protected:
		/*
		 * @param proposed size a size that a child would like to be
		 *
		 * @return a size that is clamped against the minimum size allowed for children
		 */
		float clamp_child(const FSlot& child_slot, float proposed_size) const;

		glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;
	public:

		FChildren* Get_Children() override;

	protected:
		/*
		* given the index of the dragged handle and the children, find a child above/left_of of the dragged handle that cane be resized
		* 
		* @return INDEX_NONE if no such child can be found
		*/
		static int32_t find_resizeable_slot_before_handle(int32_t dragged_handle, const TPanelChildren<FSlot>& children);

		/*
		* given the index of the dragged handle and the children, find a child below/right_of the dragged handle that can be resized
		* 
		* @return children.num() if no such child can be found
		*/
		static int32_t find_resizeable_slot_after_handle(int32_t dragged_handle, const TPanelChildren<FSlot>& children);

		static void find_all_resizeable_slots_after_handle(int32_t dragged_handle, const TPanelChildren<FSlot>& children, std::vector<int32_t>& out_slot_indices);

		/*
		* resizes the children based on user input, the template parameter orientation to the splitter being horizontal or vertical
		* 
		* @param DraggedHandle the index of the handle that the user is dragging
		* @param LocalMousePos the position of the mouse in this widgets local space
		* @param Children a reference to this splitter's children array, we will modify the children's layout values
		* @param ChildGeometries the arranged children, we need their sizes and positions so that we can perform a resizing
		*/
		void handle_resizing_by_mouse_position(EOrientation orientation, const float physical_splitter_handle_size, const ESplitterResizeMode::Type resize_mode, int32_t dragged_handle, const glm::vec2& local_mouse_pos,
			TPanelChildren<FSlot>& children, const std::vector<FLayoutGeometry>& child_geometries);

		void handle_resizing_delta(EOrientation orientation, const float physical_splitter_handle_size, const ESplitterResizeMode::Type resize_mode, int32_t dragged_handle, float delta, TPanelChildren<FSlot>& children,
			const std::vector<FLayoutGeometry>& child_geometries);

		/*
		* given a mouse position within the splitter, figure out which resize handle we are hovering (if any)
		* 
		* @param LocalMousePos the mouse position within this splitter
		* @param ChildGeometries the arranged children and their geometries, we need to test the mouse against them
		* 
		* @return the index of the handle being hovered, or index_none if we are not hovering a handle
		*/
		template<EOrientation SplitterOrientation>
		static int32_t get_handle_being_resized_from_mouse_position(float physical_splitter_handle_size,
			float hit_detection_splitter_handle_size, glm::vec2 local_mouse_pos, const std::vector<FLayoutGeometry>& child_geometries);

		TPanelChildren<FSlot> m_children;//handles

		int32_t m_hovered_handle_index;

		TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint> m_highlighted_handle_index;//handle just one splitter

		bool m_b_is_resizing;//note:when button down, this will true

		EOrientation m_orientation;
		ESplitterResizeMode::Type m_resize_mode;

		/*the user is not allowed to make any of the splitter's children smaller than this*/
		float m_min_splitter_child_length;

		/*the thickness of the grip area that the user uses to resize a splitter*/
		float m_physical_splitter_handle_size;
		float m_hit_detection_splitter_handle_size;

		const FSplitterStyle* m_style;
	};
}