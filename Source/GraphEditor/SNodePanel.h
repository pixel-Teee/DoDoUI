#pragma once

#include "SlateCore/Widgets/SPanel.h"//SPanel

#include "SlateCore/SlotBase.h"//TSlotBase

#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"//TAlignmentWidgetSlotMixin

namespace DoDo {
	/*this class is designed to serve as the base class for a panel/canvas that contains interactive widgets
	which can be selected and moved around by the user, it also manages zooming and panning, allowing a larger
	virtual space to be used for the widget placement
	
	the user is responsible for creating widgets (which must be derived from SNode) and any custom drawing
	code desired, the other main restriction is that each SNode instance must have a unique UObject* associated
	with it*/
	namespace ENodeZone
	{
		enum Type
		{
			TopLeft,
			TopCenter,
			TopRight,

			Left,
			Center,
			Right,

			BottomLeft,
			BottomCenter,
			BottomRight,

			Count
		};
	}
	struct FSlateBrush;
	class SNodePanel : public SPanel
	{
	public:

		class SNode : public SPanel
		{
			/*a slot that support alignment of content and padding and z-order*/
			class FNodeSlot : public TSlotBase<FNodeSlot>, public TAlignmentWidgetSlotMixin<FNodeSlot>
			{
			public:
				friend SNode;

				FNodeSlot()
					: FNodeSlot(ENodeZone::TopLeft)
				{}

				FNodeSlot(ENodeZone::Type in_zone)
					: TSlotBase<FNodeSlot>()
					, TAlignmentWidgetSlotMixin<FNodeSlot>(HAlign_Fill, VAlign_Fill)
					, m_zone(in_zone)
					, m_slot_padding(0.0f)
					, m_offset(glm::vec2(0.0f))
					, m_allow_scale(true)
				{}

				SLATE_SLOT_BEGIN_ARGS_OneMixin(FNodeSlot, TSlotBase<FNodeSlot>, TAlignmentWidgetSlotMixin<FNodeSlot>)
					SLATE_ATTRIBUTE(FMargin, Padding)
					SLATE_ATTRIBUTE(glm::vec2, SlotOffset)
					SLATE_ATTRIBUTE(glm::vec2, SlotSize)
					SLATE_ATTRIBUTE(bool, AllowScaling)
					SLATE_END_ARGS()

				void Construct(const FChildren& slot_owner, FSlotArguments&& in_args);
			public:
				ENodeZone::Type get_zone_type() const
				{
					return m_zone;
				}

				void set_padding(TAttribute<FMargin> in_padding)
				{
					m_slot_padding = std::move(in_padding);
				}

				FMargin get_padding() const
				{
					return m_slot_padding.Get();
				}

				void set_slot_offset(TAttribute<glm::vec2> in_offset)
				{
					m_offset = std::move(in_offset);
				}

				glm::vec2 get_slot_offset() const
				{
					return m_offset.Get();
				}

				void set_slot_size(TAttribute<glm::vec2> in_size)
				{
					m_size = std::move(in_size);
				}

				glm::vec2 get_slot_size() const
				{
					return m_size.Get();
				}

				void set_allow_scaling(TAttribute<bool> in_allow_scaling)
				{
					m_allow_scale = std::move(in_allow_scaling);
				}

				bool get_allow_scaling() const
				{
					return m_allow_scale.Get();
				}

			private:
				/*the child widget contained in this slot*/
				ENodeZone::Type m_zone;
				TAttribute<FMargin> m_slot_padding;
				TAttribute<glm::vec2> m_offset;
				TAttribute<glm::vec2> m_size;
				TAttribute<bool> m_allow_scale;
			};
		};

		SNodePanel();

		//SPanel interface
		
		//End of SPanel interface

		//SWidget interface

		//End of SWidget interface

	protected:
		//paint the background as lines
		void paint_background_as_lines(const FSlateBrush* background_image, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t& draw_layer_id) const;
	};
}