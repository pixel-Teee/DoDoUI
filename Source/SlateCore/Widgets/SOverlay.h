#pragma once

#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"//TBasicLayoutWidgetSlot
#include "SlateCore/Layout/Children.h"//TPanelChildren

#include "SlateCore/Widgets/SPanel.h"

namespace DoDo
{
	/*
	 * implements an overlay widget
	 * overlay widgets allow for layering several widgets on top of each other
	 * each slot of an overlay represents a layer that can contain one widget
	 * the slots will be renderer on top of each other in the order they are declared in code
	 *
	 * usage:
	 *	SNew(SOverlay)
	 *	+ SOverlay::Slot(SNew(SMyWidget1))
	 *	+ SOverlay::Slot(SNew(SMyWidget2))
	 *	+ SOverlay::Slot(SNew(SMyWidget3))
	 *
	 *	note that SWidget3 will be drawn on top of SWidget2 and SWidget3
	 */
	class SOverlay : public SPanel
	{
		SLATE_DECLARE_WIDGET(SOverlay, SPanel)
	public:
		class FOverlaySlot : public TBasicLayoutWidgetSlot<FOverlaySlot>
		{
		public:
			FOverlaySlot(int32_t in_zorder)
				: TBasicLayoutWidgetSlot<FOverlaySlot>(HAlign_Fill, VAlign_Fill)
				, m_zorder(in_zorder)
			{}

			SLATE_SLOT_BEGIN_ARGS(FOverlaySlot, TBasicLayoutWidgetSlot<FOverlaySlot>)

			SLATE_SLOT_END_ARGS()

			void Construct(const FChildren& slot_owner, FSlotArguments&& in_args);

			int32_t get_zorder() const
			{
				return m_zorder;
			}

			void set_zorder(int32_t in_zorder);
		public:
			int32_t m_zorder;
		};

		SLATE_BEGIN_ARGS(SOverlay)
		{
			//todo:visibility	
		}
		SLATE_SLOT_ARGUMENT(SOverlay::FOverlaySlot, Slots)
		SLATE_END_ARGS()

		SOverlay();

		/*
		 * construct this widget
		 *
		 * @param InArgs the declaration data for this widget
		 */
		void Construct(const FArguments& in_args);

		/*@return a new slot, slots contain children for SOverlay*/
		static FOverlaySlot::FSlotArguments Slot();

		//------begin of SWidget interface------
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;
		virtual FChildren* Get_Children() override;
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
		//------begin of SWidget interface------
	protected:
		glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;

	protected:
		/*the SOverlay's slots, each slot contains a child widget*/
		TPanelChildren<FOverlaySlot> m_children;
	};
}