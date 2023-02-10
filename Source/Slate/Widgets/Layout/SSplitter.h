#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/SlotBase.h"//FSlot depends on it
#include "SlateCore/Layout/Children.h"//TPanelChildren
#include "SlateCore/Layout/LayoutGeometry.h"//FLayoutGeometry depends on it
#include "SlateCore/Styling/CoreStyle.h"
#include "SlateCore/Styling/ISlateStyle.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
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

		using FOnSlotResized = Delegate<void(float)>;//the new size coefficient of the slot

		using FOnGetMaxSlotSize = Delegate<glm::vec2(int32_t)>;

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

			float get_min_size() const
			{
				return m_min_size_value.Get(0.0f);
			}

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

		SLATE_BEGIN_ARGS(SSplitter)
			: _Style(&FCoreStyle::get().get_widget_style<FSplitterStyle>("Splitter"))
			, _Orientation(Orient_Horizontal)
			, _PhysicalSplitterHandleSize(5.0f)
			, _HitDetectionSplitterHandleSize(5.0f)
			, _MinimumSlotHeight(20.0f)
			{}
			SLATE_SLOT_ARGUMENT(FSlot, Slots)

			/*style used to draw this splitter*/
			SLATE_STYLE_ARGUMENT(FSplitterStyle, Style)

			SLATE_ARGUMENT(EOrientation, Orientation)

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

	public:
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
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
		TPanelChildren<FSlot> m_children;//handles

		int32_t m_hovered_handle_index;

		TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint> m_highlighted_handle_index;//handle just one splitter

		EOrientation m_orientation;

		/*the user is not allowed to make any of the splitter's children smaller than this*/
		float m_min_splitter_child_length;

		/*the thickness of the grip area that the user uses to resize a splitter*/
		float m_physical_splitter_handle_size;
		float m_hit_detection_splitter_handle_size;

		const FSplitterStyle* m_style;
	};
}