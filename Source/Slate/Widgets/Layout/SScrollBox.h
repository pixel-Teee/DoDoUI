#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/Types/SlateEnums.h"//EOrientation depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "SlateCore/Widgets/SCompoundWidget.h"

namespace DoDo {
	/*SScrollBox can scroll through an arbitrary number of widgets*/
	class SScrollBox : public SCompoundWidget
	{
	public:
		/*a slot that provides layout options for the contents of a scrollable box*/
		using FSlot = FBasicLayoutWidgetSlot;


	};

	class SScrollPanel : public SPanel
	{
	public:

		SLATE_BEGIN_ARGS(SScrollPanel)
		{

		}

		SLATE_ARGUMENT(EOrientation, Orientation)
			SLATE_ARGUMENT(bool, BackPadScrolling)
			SLATE_ARGUMENT(bool, FrontPadScrolling)
			SLATE_END_ARGS()

			void Construct(const FArguments& in_args, std::vector<SScrollBox::FSlot::FSlotArguments> in_slots);

		EOrientation get_orientation()
		{
			return m_orientation;
		}

		void set_orientation(EOrientation in_orientation)
		{
			m_orientation = in_orientation;
		}

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual FChildren* Get_Children() override
		{
			return &m_children;
		}

		float m_physical_offset;
		TPanelChildren<SScrollBox::FSlot> m_children;

	protected:
		virtual glm::vec2 Compute_Desired_Size(float) const override;

	private:
		float arrange_child_vertical_and_return_offset(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, const SScrollBox::FSlot& this_slot, float cur_child_offset) const;

		float arrange_child_horizontal_and_return_offset(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, const SScrollBox::FSlot& this_slot, float cur_child_offset) const;

		EOrientation m_orientation;

		bool m_back_pad_scrolling;

		bool m_front_pad_scrolling;
	};
}