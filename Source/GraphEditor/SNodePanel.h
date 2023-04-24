#pragma once

#include "SlateCore/Widgets/SPanel.h"//SPanel

#include "SlateCore/SlotBase.h"//TSlotBase

#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"//TAlignmentWidgetSlotMixin

#include "SlateCore/Layout/Children.h"//FChildren depends on it

#include "SlateCore/Layout/ArrangedChildren.h"//FArrangedChildren depends on it

#include "SlateCore/Layout/LayoutUtils.h"//AlignmentArrangeResult

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

		class SNode : public SPanel //SNode is composed of many sub nodes
		{
		public:
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

			typedef std::set<std::weak_ptr<SNodePanel::SNode>> FNodeSet;

			//SPanel interface
			virtual FChildren* Get_Children() override
			{
				return &m_children;
			}

			virtual glm::vec2 Compute_Desired_Size(float) const override
			{
				for (int32_t child_index = 0; child_index < m_children.num(); ++child_index)
				{
					if (m_children[child_index].m_zone == ENodeZone::Center)
					{
						const FNodeSlot& center_zone = m_children[child_index];

						const EVisibility child_visibility = center_zone.get_widget()->get_visibility();

						if (child_visibility != EVisibility::Collapsed)
						{
							return (center_zone.get_widget()->get_desired_size() + center_zone.m_slot_padding.Get().get_desired_size()) * m_desired_size_scale.Get();
						}
					}
				}

				return glm::vec2(0.0f);
			}

			virtual float Get_Relative_Layout_Scale(const int32_t child_index, float layout_scale_multiplier) const override
			{
				const FNodeSlot& this_slot = m_children[child_index];
				if (!this_slot.m_allow_scale.Get())
				{
					//child slots that do not allow zooming should scale themselves to negate the node panel's zoom
					std::shared_ptr<SNodePanel> parent_panel = get_parent_panel();

					if (parent_panel)
					{
						return 1.0f / parent_panel->get_zoom_amount();
					}
				}

				return 1.0f;
			}

			virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override
			{
				for (int32_t child_index = 0; child_index < m_children.num(); ++child_index)
				{
					const FNodeSlot& cur_child = m_children[child_index];
					const EVisibility child_visibility = cur_child.get_widget()->get_visibility();
					if (arranged_children.Accepts(child_visibility))
					{
						const FMargin slot_padding(cur_child.m_slot_padding.Get());

						//if this child is not allowed to scale, its scale relative to its parent should undo the parent widget's scaling
						glm::vec2 Size;

						if (cur_child.m_size.Is_Set())
						{
							Size = cur_child.m_size.Get();
						}
						else
						{
							AlignmentArrangeResult x_result = Align_Child<Orient_Horizontal>(allotted_geometry.get_local_size().x, cur_child, slot_padding);
							AlignmentArrangeResult y_result = Align_Child<Orient_Vertical>(allotted_geometry.get_local_size().y, cur_child, slot_padding);

							Size = glm::vec2(x_result.m_size, y_result.m_size);
						}

						const FArrangedWidget child_geom =
							allotted_geometry.make_child(
								cur_child.get_widget(),
								cur_child.m_offset.Get(),
								Size,
								Get_Relative_Layout_Scale(child_index, allotted_geometry.m_scale)
							);

						arranged_children.add_widget(child_visibility, child_geom);
					}
				}
			}

		protected:
			//SBorder Begin
			TAttribute<glm::vec2> m_desired_size_scale;
			//SBorder End

		private:

			std::shared_ptr<SNodePanel> get_parent_panel() const
			{
				return m_parent_panel_ptr.lock();
			}

			TPanelChildren<FNodeSlot> m_children;
			std::weak_ptr<SNodePanel> m_parent_panel_ptr;
		};

		SNodePanel();

		//SPanel interface
		
		//End of SPanel interface

		//SWidget interface

		//End of SWidget interface

	public:
		/*@return the zoom amount, e.g. a value of 0.25f results in quarter-sized nodes*/
		float get_zoom_amount() const;

	protected:
		//paint the background as lines
		void paint_background_as_lines(const FSlateBrush* background_image, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t& draw_layer_id) const;
	};
}