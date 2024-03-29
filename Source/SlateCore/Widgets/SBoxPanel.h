#pragma once

#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"//TBasicLayoutWidgetSlot
#include "SlateCore/Layout/Children.h"

#include "SlateCore/Widgets/SPanel.h"//TPanelChildren

#include "SlateCore/Types/SlateStructs.h"//FSizeParam

namespace DoDo {
	/*
	* a box panel contains one child and describes how that child should be arranged on the screen
	*/
	class SBoxPanel : public SPanel
	{
		SLATE_DECLARE_WIDGET(SBoxPanel, SPanel)

	protected:
		/*
		* a box panel contains one box panel child and describes how that
		* child should be arranged on the screen
		*/
		template<typename SlotType>
		class TSlot : public TBasicLayoutWidgetSlot<SlotType>
		{
		public:
			SLATE_SLOT_BEGIN_ARGS(TSlot, TBasicLayoutWidgetSlot<SlotType>)
				SLATE_ARGUMENT(std::optional<FSizeParam>, SizeParam)
				TAttribute<float> _MaxSize;
			SLATE_SLOT_END_ARGS()

		protected:
			/*default values for a slot*/
			TSlot()
				: TBasicLayoutWidgetSlot<SlotType>(HAlign_Fill,VAlign_Fill)
				, m_size_rule(FSizeParam::SizeRule_Stretch)
				, m_size_value(*this, 1.0f)
				, m_max_size(*this, 1.0f)
			{}

		public:
			void Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
			{
				TBasicLayoutWidgetSlot<SlotType>::Construct(slot_owner, std::move(in_args));

				if(in_args._MaxSize.Is_Set())
				{
					set_max_size(std::move(in_args._MaxSize));
				}

				if(in_args._SizeParam.has_value())
				{
					set_size_param(std::move(in_args._SizeParam.value()));
				}
			}

			/*get the space rule this slot should occupy along panel's direction*/
			FSizeParam::ESizeRule get_size_rule() const
			{
				return m_size_rule;
			}

			/*get the space rule value this slot should occupy along panel's direction*/
			float get_size_value() const
			{
				return m_size_value.Get();
			}

			/*get the max size the slot can be*/
			float get_max_size() const
			{
				return m_max_size.Get();
			}

			/*set the size param of the slot, it could be a FStretch or a FAuto*/
			void set_size_param(FSizeParam in_size_param)
			{
				m_size_rule = in_size_param.m_size_rule;

				m_size_value.Assign(*this, std::move(in_size_param.m_value));
			}

			/*set the max size in SlateUnit this slot can be*/
			void set_max_size(TAttribute<float> in_max_size)
			{
				m_max_size.Assign(*this, std::move(in_max_size));
			}

		private:
			/*
			 * how much space this slot should occupy along panel's direction
			 * when SizeRule is SizeRule_Auto, the widget's DesiredSize will be used as the space required
			 * when SizeRule is SizeRule_Stretch, the available space will be distributed proportionately between
			 * peer widgets depending on the value property, available space is space remaining after all the
			 * peer's SizeRule_Auto requirements have been satisfied
			 */

			/*the sizing rule to use*/
			FSizeParam::ESizeRule m_size_rule;

			/*the actual value this size parameter stores*/
			//note:use TBasicLayoutWidgetSlot's slot attribute type
			typename TBasicLayoutWidgetSlot<SlotType>::template TSlateSlotAttribute<float> m_size_value;

			/*the max size that this slot can be (0 if no max)*/
			typename TBasicLayoutWidgetSlot<SlotType>::template TSlateSlotAttribute<float> m_max_size;
		};

	public:
		class FSlot : public TSlot<FSlot>
		{
			
		};

	public:
		/*begin SWidget overrides*/
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual FChildren* Get_Children() override;
		/*end SWidget overrides*/

	protected:
		glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;

		/*
		 * a box panel's orientation cannot be changed once it is constructed
		 *
		 * @param InOrientation the orientation of the box panel
		 */
		SBoxPanel(EOrientation in_orientation);

		/*the box panel's children*/
		TPanelChildren<FSlot> m_children;

		/*the box panel's orientation, determined at construct time*/
		const EOrientation m_orientation;
	};

	/*a horizontal box panel, see SBoxPanel for more info*/
	class SHorizontalBox : public SBoxPanel
	{
		SLATE_DECLARE_WIDGET(SHorizontalBox, SBoxPanel)
	public:
		class FSlot : public SBoxPanel::TSlot<FSlot>
		{
		public:
			SLATE_SLOT_BEGIN_ARGS(FSlot, SBoxPanel::TSlot<FSlot>)
			/*the widget's desired size will be used as the space required*/
			FSlotArguments& auto_width()
			{
				_SizeParam = FAuto();//FAuto inherited from FSizeParam
				return Me();
			}
			/*the available space will be distributed proportionately*/
			FSlotArguments& fill_width(TAttribute<float> in_stretch_coefficient)
			{
				_SizeParam = FStretch(std::move(in_stretch_coefficient));
				return Me();
			}
			/*set the max size in slate unit this slot can be*/
			FSlotArguments& max_width(TAttribute<float> in_max_width)
			{
				_MaxSize = std::move(in_max_width);
				return Me();
			}
			SLATE_SLOT_END_ARGS()
		};

		static FSlot::FSlotArguments Slot()
		{
			return FSlot::FSlotArguments(std::make_unique<FSlot>());
		}

		SLATE_BEGIN_ARGS(SHorizontalBox)
		{}
		SLATE_SLOT_ARGUMENT(SHorizontalBox::FSlot, Slots)
		SLATE_END_ARGS()

		SHorizontalBox()
			: SBoxPanel(Orient_Horizontal)
		{
			//todo:implement set can tick
		}

		/*
		 * construct this widget
		 *
		 * @param InArgs the declaration data for this widget
		 */
		void Construct(const FArguments& in_args);
	};
}