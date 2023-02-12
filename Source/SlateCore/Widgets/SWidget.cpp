#include <PreCompileHeader.h>

#include "SWidget.h"

#include "DeclarativeSyntaxSupport.h"
#include "SlateCore/Input/HittestGird.h"
#include "SlateCore/Types/ISlateMetaData.h"

#include "SlateCore/Types/SlateAttributeDescriptor.h"

#include "SlateCore/Types/PaintArgs.h"//FPaintArgs depends on it
#include "SlateCore/Types/SlateMouseEventsMetaData.h"//FSlateMouseEventsMetaData depends on it

namespace DoDo {
	//this function will be called at FSlateWidgetClassData construct
	void SWidget::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//just to register some static information of slate attribute
		//visibility should be the first attribute in the list
		//the order in which slate attribute are declared in the .h dictates of the order
		//attribute name
		//calculate SWidget and m_Visibility_Attribute offset
		//FInvalidateWidgetReasonAttribute(may be with callback)
		//attribute_initializer.add_member_attribute("Visibility", STRUCT_OFFSET(PrivateThisType, m_Visibility_Attribute), FSlateAttributeDescriptor::FInvalidateWidgetReasonAttribute{EInvalidateWidgetReason::Visibility}).affect_visibility();
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "Visibility", m_Visibility_Attribute, EInvalidateWidgetReason::Visibility).affect_visibility();
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "EnabledState", m_enabled_state_attribute, EInvalidateWidgetReason::Paint);

		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "RenderTransformPivot", m_render_transform_pivot_attribute, EInvalidateWidgetReason::Layout | EInvalidateWidgetReason::Render_Transform);
	}

	SWidget::~SWidget()
	{
		m_b_has_registered_slate_attribute = false;//set this bool variable
	}

	glm::vec2 SWidget::get_desired_size() const
	{
		return glm::vec2(m_desired_size.value_or(glm::vec2(0.0f)));
	}

	void SWidget::assign_parent_widget(std::shared_ptr<SWidget> in_parent)
	{
		m_parent_widget_ptr = in_parent;

		if(in_parent)
		{
			in_parent->Invalidate(EInvalidateWidgetReason::Child_Order);
		}
	}

	bool SWidget::conditionally_detach_parent_widget(SWidget* in_expected_parent)
	{
		std::shared_ptr<SWidget> parent = m_parent_widget_ptr.lock();

		if(parent.get() == in_expected_parent)
		{
			m_parent_widget_ptr.reset();

			//give a flag
			if(parent != nullptr)
			{
				parent->Invalidate(EInvalidateWidgetReason::Child_Order);
			}

			invalidate_child_remove_from_tree(*this);

			return true;
		}

		return false;
	}

	void SWidget::Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children,
		bool b_update_attributes) const
	{
		if(b_update_attributes)
		{
			//todo:implement FSlateAttributeMetaData::UpdateChildrenOnlyVisibilityAttributes
		}

		//call virtual function OnArrangeChildren
		On_Arrange_Children(allotted_geometry, arranged_children);
	}

	//FSlateAttributeMetaData::remove_meta_data_if_needed will call this function
	//interms of parameter to mark different flag
	void SWidget::Invalidate(EInvalidateWidgetReason in_validate_reason)
	{
		//don't have constructed
		if(in_validate_reason == EInvalidateWidgetReason::None || !Is_Constructed())
		{
			return;
		}

		if(Enum_Has_Any_Flags(in_validate_reason, EInvalidateWidgetReason::Prepass))
		{
			//todo:implement mark prepass as dirty function
			in_validate_reason |= EInvalidateWidgetReason::Layout;
		}

		if(Enum_Has_Any_Flags(in_validate_reason, EInvalidateWidgetReason::Child_Order))
		{
			//todo:implement mark prepass as dirty function
			in_validate_reason |= EInvalidateWidgetReason::Prepass;
			in_validate_reason |= EInvalidateWidgetReason::Layout;
		}

		//todo:implement left part
	}

	const FGeometry& SWidget::get_paint_space_geometry() const
	{
		return m_persistent_state.m_allotted_geometry;
	}

	namespace Private
	{
		std::shared_ptr<FSlateMouseEventsMetaData> find_or_add_mouse_events_meta_data(SWidget* widget)
		{
			std::shared_ptr<FSlateMouseEventsMetaData> data = widget->get_meta_data<FSlateMouseEventsMetaData>();

			if(!data)
			{
				data = std::make_shared<FSlateMouseEventsMetaData>();
				widget->add_meta_data(data);
			}
			return data;
		}
	}

	void SWidget::add_meta_data_internal(const std::shared_ptr<ISlateMetaData>& add_me)
	{
		m_Meta_Data.push_back(add_me);
	}

	void SWidget::set_on_mouse_move(FPointerEventHandler event_handler)
	{
		Private::find_or_add_mouse_events_meta_data(this)->m_mouse_move_handler = event_handler;
	}

	SWidget::SWidget()
		: m_b_is_hovered_attribute_set(false)
		, m_hovered_attribute(*this, false)
		, m_b_has_registered_slate_attribute(false)
		, m_Visibility_Attribute(*this, EVisibility::visible)
		, m_enabled_state_attribute(*this, true)
		, m_render_transform_pivot_attribute(*this, glm::vec2(0.0f))
		, m_render_transform_attribute(*this, glm::vec2(0.0f, 0.0f))
	{
	}

	void SWidget::SWidgetConstruct(const FSlateBaseNamedArgs& args)
	{
		//todo:to fix this
		//m_Visibility_Attribute.Assign(*this, args._Visibility);

		//------append a array to this array------
		for(size_t i = 0; i < args.m_meta_data.size(); ++i)
		{
			m_Meta_Data.push_back(args.m_meta_data[i]);
		}
		//------append a array to this array------
	}

	int32_t SWidget::paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		SWidget* mutable_this = const_cast<SWidget*>(this);

		FGeometry desktop_space_geometry = allotted_geometry;
		//todo:implement this variable

		//------collect some information for handle input------
		//todo:first to get the paint arg's widget, then update paint args's widget
		//to construct a widget chain
		SWidget* paint_parent_ptr = const_cast<SWidget*>(args.get_paint_parent());//todo:fix me

		if(paint_parent_ptr)
		{
			m_persistent_state.m_paint_parent = paint_parent_ptr->shared_from_this();//todo:fix me?
		}
		else
		{
			paint_parent_ptr = nullptr;
		}

		m_persistent_state.m_layer_id = layer_id;
		m_persistent_state.m_b_parent_enabled = b_parent_enabled;
		m_persistent_state.m_allotted_geometry = allotted_geometry;
		m_persistent_state.m_desktop_geometry = desktop_space_geometry;
		m_persistent_state.m_widget_style = in_widget_style;

		//todo:assign user index
		m_persistent_state.m_incoming_flow_direction = g_slate_flow_direction;
		//------collect some information for handle input------

		args.get_hittest_grid().add_widget(mutable_this, 0, layer_id);//todo:implement widget sort order

		//todo:update paint args
		FPaintArgs updated_args = args.with_new_parent(this);

		//paint the geometry of this widget
		int32_t new_layer_id = On_Paint(updated_args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);

		return new_layer_id;
	}

	FReply SWidget::On_Key_Down(const FGeometry& my_geometry, const FKeyEvent& in_key_event)
	{
		//todo:implement support focus

		return FReply::un_handled();
	}

	FReply SWidget::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (std::shared_ptr<FSlateMouseEventsMetaData> data = get_meta_data<FSlateMouseEventsMetaData>())
		{
			if (data->m_mouse_button_down_handle.is_bound())
			{
				return data->m_mouse_button_down_handle.execute(my_geometry, mouse_event);
			}
		}

		return FReply::un_handled();
	}

	FReply SWidget::On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (std::shared_ptr<FSlateMouseEventsMetaData> data = get_meta_data<FSlateMouseEventsMetaData>())
		{
			if (data->m_mouse_button_up_handle.is_bound())
			{
				return data->m_mouse_button_up_handle.execute(my_geometry, mouse_event);
			}
		}

		return FReply::un_handled();
	}

	FReply SWidget::On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		//todo:get mouse events meta data to handle
		if (std::shared_ptr<FSlateMouseEventsMetaData> data = get_meta_data<FSlateMouseEventsMetaData>())
		{
			if (data->m_mouse_move_handler.is_bound())
			{
				return data->m_mouse_move_handler.execute(my_geometry, mouse_event);
			}
		}

		return FReply::un_handled();
	}

	void SWidget::invalidate_child_remove_from_tree(SWidget& child)
	{
		//todo:implement this function
	}
}

