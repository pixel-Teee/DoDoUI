#pragma once

#include "SlateCore/Widgets/SPanel.h"//SPanel

#include "SlateCore/SlotBase.h"//TSlotBase

#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"//TAlignmentWidgetSlotMixin

#include "SlateCore/Layout/Children.h"//FChildren depends on it

#include "SlateCore/Layout/ArrangedChildren.h"//FArrangedChildren depends on it

#include "SlateCore/Layout/LayoutUtils.h"//AlignmentArrangeResult

#include "SlateCore/Types/PaintArgs.h"

#include "SlateCore/Styling/SlateColor.h"//FSlateColor depends on it

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

			virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override
			{
				FArrangedChildren arranged_children(EVisibility::visible);
				{
					Arrange_Children(allotted_geometry, arranged_children);
				}

				int32_t max_layer_id = layer_id;

				for(int32_t child_index = 0; child_index < arranged_children.num(); ++child_index)
				{
					const FArrangedWidget& cur_widget = arranged_children[child_index];

					if(!is_child_widget_culled(my_culling_rect, cur_widget))
					{
						const int32_t cur_widgets_max_layer_id = cur_widget.m_widget->paint(args.with_new_parent(this), cur_widget.m_geometry, my_culling_rect, out_draw_elements,
							layer_id, in_widget_style, should_be_enabled(b_parent_enabled));
					}
					else
					{
						//slate gi - remove content
					}
				}

				return max_layer_id;
			}

			//end of SPanel interface
			using FScopedWidgetSlotArguments = TPanelChildren<FNodeSlot>::FScopedWidgetSlotArguments;

			FScopedWidgetSlotArguments get_or_add_slot(const ENodeZone::Type slot_id)
			{
				//return existing
				int32_t insert_index = -1;
				for (int32_t child_index = 0; child_index < m_children.num(); ++child_index)
				{
					if (m_children[child_index].m_zone == slot_id)
					{
						m_children.remove_at(child_index);
						insert_index = child_index;
					}
				}

				//add new
				return FScopedWidgetSlotArguments{ std::make_unique<FNodeSlot>(slot_id), m_children, insert_index };
			}

			FNodeSlot* get_slot(const ENodeZone::Type slot_id)
			{
				FNodeSlot* result = nullptr;
				//return existing
				for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
				{
					if(m_children[child_index].m_zone == slot_id)
					{
						result = &m_children[child_index];
						break;
					}
				}
				return result;
			}

			void remove_slot(const ENodeZone::Type slot_id)
			{
				for(int32_t child_index = 0; child_index < m_children.num(); ++child_index)
				{
					if(m_children[child_index].m_zone == slot_id)
					{
						m_children.remove_at(child_index);
						break;
					}
				}
			}

			/*
			 * @param NewPosition the node should be relocated to this position in the graph panel
			 * @param NodeFilter set of nodes to prevent movement on, after moving successfully a node is added to this set
			 * @param bMarkDirty if we should mark nodes as dirty on move
			 */
			virtual void move_to(const glm::vec2& new_position, FNodeSet& node_filter, bool b_mark_dirty = true)
			{
				
			}

			/*@return the node's position within the graph*/
			virtual glm::vec2 get_position() const
			{
				return glm::vec2(0.0f, 0.0f);
			}

			/*@return a user-specified comment on this node, the comments gets drawn in a bubble above the node*/
			virtual DoDoUtf8String get_node_comment() const
			{
				return DoDoUtf8String();
			}

			//todo:return the backing object

			/*@return the brush to use for drawing the shadow for this node*/
			virtual const FSlateBrush* get_shadow_brush(bool b_selected) const
			{
				return nullptr;//todo:implement FEditorStyle to get graph.node.shadow selected
			}

			//todo:implement get node info popups

			//todo:implement require second pass layout

			void set_parent_panel(const std::shared_ptr<SNodePanel>& in_parent)
			{
				m_parent_panel_ptr = in_parent;
			}
		protected:
			SNode()
				: m_desired_size_scale(glm::vec2(1, 1))
				, m_children(this)
			{
				//todo:add has relative layout scale
			}
		protected:
			//SBorder Begin
			TAttribute<const FSlateBrush*> m_border_image;
			TAttribute<FSlateColor> m_border_background_color;
			TAttribute<glm::vec2> m_desired_size_scale;
			/*whether or not show the disabled effect when this border is disabled*/
			TAttribute<bool> m_shadow_disabled_effect;
			/*mouse event handler*/
			FPointerEventHandler m_mouse_button_down_handler;
			FPointerEventHandler m_mouse_button_up_handler;
			FPointerEventHandler m_mouse_move_handler;
			FPointerEventHandler m_mouse_double_click_handler;
			//SBorder End

			//SPanel Begin
			/*the layout scale to apply to this widget's contents, useful for animation*/
			TAttribute<glm::vec2> m_content_scale;

			/*the color and opacity to this widget and all its descendants*/
			TAttribute<FLinearColor> m_color_and_opacity;

			/*optional foreground that will be inherited by all of this widget's contents*/
			TAttribute<FSlateColor> m_foreground_color;
			//SPanel End
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
