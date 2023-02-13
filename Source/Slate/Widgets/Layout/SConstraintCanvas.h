#pragma once

#include "SlateCore/SlotBase.h"//SConstraintCanvas::FSlot depends on it

//#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"//SLATE_SLOT_BEGIN_ARGS

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/Layout/Margin.h"

#include "Anchors.h"

#include <vector>

#include "SlateCore/Layout/Children.h"//TPanelChildren depends on it

namespace DoDo
{
	/*
	 * constraint canvas is a layout widgets that allows you to arbitrary position and size child widgets in a
	 * relative coordinate space, additionally it permits anchoring widgets
	 */
	class SConstraintCanvas : public SPanel
	{
		/*
		 * constraint canvas slots allow child widgets to be positioned and sized
		 */
		class FSlot : public TSlotBase<FSlot>//FSlot is a slot that have extra information
		{
		public:
			/*defaults values for a slot*/
			FSlot()
				: TSlotBase<FSlot>()
				, m_offset_attr(FMargin(0.0f, 0.0f, 1.0f, 1.0f))
				, m_anchors_attr(FAnchors())
				, m_alignment_attr(glm::vec2(0.5f, 0.5f))
				, m_zorder(0)
				, m_auto_size_attr(false)
			{}

			SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
				SLATE_ATTRIBUTE(FMargin, Offset)
				SLATE_ATTRIBUTE(FAnchors, Anchors)
				SLATE_ATTRIBUTE(glm::vec2, Alignment)
				SLATE_ATTRIBUTE(bool, AutoSize)
				SLATE_ARGUMENT(std::optional<float>, ZOrder)
			SLATE_SLOT_END_ARGS()

			void Construct(const FChildren& slot_owner, FSlotArguments&& in_args);

			//todo:implement other set and get function
			FMargin get_offset() const
			{
				return m_offset_attr.Get();
			}

			FAnchors get_anchors() const
			{
				return m_anchors_attr.Get();
			}
			glm::vec2 get_alignment() const
			{
				return m_alignment_attr.Get();
			}

			bool get_auto_size() const
			{
				return m_auto_size_attr.Get();
			}

			float get_zorder() const
			{
				return m_zorder;
			}

		public:
			/*offset*/
			TAttribute<FMargin> m_offset_attr;

			/*anchors*/
			TAttribute<FAnchors> m_anchors_attr;

			/*size*/
			TAttribute<glm::vec2> m_alignment_attr;

			/*auto-size*/
			TAttribute<bool> m_auto_size_attr;

			/*z-order*/
			TAttribute<float> m_zorder_attr;//todo:this is editor only data

		private:
			float m_zorder;
		};

		SLATE_BEGIN_ARGS(SConstraintCanvas)//note:this implement a FArguments, have a FSlot::FSlotArguments array
			{
				//todo:implement visibility
			}
			SLATE_SLOT_ARGUMENT(FSlot, Slots)
		SLATE_END_ARGS()

		SConstraintCanvas();

		static FSlot::FSlotArguments Slot();

		/*
		 * construct this widget
		 *
		 * @param InArgs the declaration data for this widget
		 */
		void Construct(const FArguments& in_args);

		//------begin swidget overrides------
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;;

		virtual FChildren* Get_Children() override;
		//------begin swidget overrides------

	private:
		//an array matching the length and order of arranged children, true means the child must be replace in a layer in front of all previous children
		typedef std::vector<bool> FArrangedChildLayers;

		/*like arrange children but also generates an array of layering information(see FArrangedChildrenLayers)*/
		void arrange_layered_children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, FArrangedChildLayers& arranged_child_layers) const;

	protected:

		glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;
		
	private:

		/*the constraint canvas widget's children*/
		TPanelChildren<FSlot> m_children;
	};

}
