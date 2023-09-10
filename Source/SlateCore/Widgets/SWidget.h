#pragma once

#include <Core/Core.h>

#include <type_traits>

#include "SlateCore/Types/SlateAttribute.h"
#include "SlateCore/Widgets/SlateControlledConstruction.h"

#include "SlateCore/Layout/Visibility.h"//m_Visibility_Attribute depends on it(EVisibility)

#include "glm/glm.hpp"

#include "SlateCore/Rendering/SlateRenderTransform.h"//m_render_transform_attribute depends on it

#include <optional>//std::optional depends on it

#include "ApplicationCore/GenericPlatform/GenericApplicationMessageHandler.h"//EWindowZone depends on it
#include "SlateCore/Input/Reply.h"
#include "SlateCore/FastUpdate/WidgetProxy.h"
#include "SlateCore/Layout/FlowDirection.h"
#include "SlateCore/Types/ISlateMetaData.h"
#include "SlateCore/Types/WidgetMouseEventsDelegate.h"//FPointerEventHandler depends on it

#include "SlateCore/Input/CursorReply.h"//FCursorReply depends on it

#include "SlateCore/Layout/Clipping.h"//EWidgetClipping depends on it

#include "SlateCore/Layout/Margin.h"//FMargin depends on it

namespace DoDo
{
	struct FKeyEvent;
	struct FSlateBaseNamedArgs;
	class FChildren;

	class ISlateMetaData;

	class FPaintArgs;
	struct FGeometry;
	class FSlateRect;
	class FSlateWindowElementList;
	class FWidgetStyle;
	class FArrangedChildren;
	struct FPointerEvent;
	class FDragDropEvent;
	class SWidget : public FSlateControlledConstruction, public std::enable_shared_from_this<SWidget>
	{
		friend class SWindow;
	public:
		//widgets should only ever be constructed via SNew or SAssignNew
		//todo:need to implement Private_Register Attributes static function
		SLATE_DECLARE_WIDGET(SWidget, FSlateControlledConstruction)

		friend class FSlateAttributeMetaData;

		template<class WidgetType, typename RequiredArgsPayloadType>
		friend struct TSlateDecl;//for SWidget construct use

		//SWidget();

	public:
		virtual ~SWidget() override;

	public:
		//layout

		/* return true if the widgets will update its registered slate attributes automatically or they need to be updated manually */
		bool Is_Attributes_Updates_Enabled() const { return m_b_enabled_attributes_update; }

		/* return the desired size that was computed the last time CachedDesiredSize() was called */
		glm::vec2 get_desired_size() const;

		std::shared_ptr<SWidget> get_parent_widget() const { return m_parent_widget_ptr.lock(); }

		/*
		* calculates what if any clipping state changes need to happen when drawing this widget
		* @return the culling rect that should be used going forward
		*/
		FSlateRect calculate_culling_and_clipping_rules(const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, bool& b_clip_to_bounds, bool& b_always_clip,
			bool& b_intersect_clip_bounds) const;

		std::shared_ptr<SWidget> advanced_get_paint_parent_widget() const { return m_persistent_state.m_paint_parent.lock(); }//todo:may be to check

		void assign_parent_widget(std::shared_ptr<SWidget> in_parent);
		/* be used by FSlotBase to detach this widget from parent widget*/
		bool conditionally_detach_parent_widget(SWidget* in_expected_parent);

	public:
		/*
		 * the system calls this method, it performs a breadth-first traversal of every visible widget and asks
		 * each widget to cache how bit it needs to be in order to present all of its content
		 */
		virtual void cache_desired_size(float in_layout_scale_multiplier);


		/*
		 * compute the ideal size necessary to display this widget. for aggregate widgets (e.g. panels) this size should include the
		 * size necessary to show all of it's children. CacheDesiredSize() guarantess that the size of descendatnts is computed and cached
		 * before that of the parents, so it is safe to call GetDesiredSize() for any children while implementing this method
		 *
		 * note that ComputeDesiredSize() is meant as an aide to the developer. it is not meant to be very roubust in many case
		 *
		 * @param LayoutScaleMultiplier this parameter is safe to ignore for almost all widgets, only really affects text measuring
		 *
		 * @return the desired size
		 */
		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const = 0;

		/*what is the child's scale relative to this widget*/
		virtual float Get_Relative_Layout_Scale(const int32_t child_index, float layout_scale_multiplier) const;

		/*
		 * non-virtual entry point for arrange children
		 * arrange children function
		 * compute the geometry of all the children and add populate the arranged children list with their values
		 * each type of layout panel should arrange children based on desired behaviour
		 *
		 * optionally, update the collapsed attributes(attributes that affect the visibility) of the children before executing the virtual arrange children function
		 * the visibility attribute is updated once per frame(see slate pre pass)
		 * use the option when you are calling arrange children outside of the regular SWidget Paint/Tick
		 *
		 * @param AllottedGeometry the geometry allotted for this widget by it's parent
		 * @param ArrangedChildren the array to which to add the widget geometries that represent the arranged children
		 * @param bUpdateAttributes update the collapsed attributes
		 */
		void Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, bool b_update_attributes = false) const;

		/*
		 * returns the useful children(if any) of this widget. some widget type may hide widget if they are needed by the system.
		 * allows for iteration over the widget's children regardless of how they are actually stored
		 * @note should be renamed to GetVisibleChidlren(not all children will be returned in all cases)
		 */
		virtual FChildren* Get_Children() = 0;

		/*
		* checks to see if this widget supports keyboard focus, override this in derived classes
		* 
		* @return true if this widget can take keyboard focus
		*/
		virtual bool supports_key_board_focus() const;

		/*
		* checks to see if this widget currently has the keyboard focus
		* 
		* @return true if this widget has keyboard focus
		*/
		virtual bool has_keyboard_focus() const;

		/*
		* gets whether or not any users have this widget focused, and if so the type of focus (first one found)
		* 
		* @return the optional will be set with the focus cause, if unset this widget doesn't have focus
		*/
		std::optional<EFocusCause> has_any_user_focus() const;

		/*
		 * compute the geometry of all the children and add populate the arranged children list with their values
		 * each type of layout panel should arrange children based on desired behaviour
		 *
		 * @param AllottedGeometry the geometry allotted for this widget by it's parent
		 * @param ArrangedChildren the array to which to add the widget geometries that represent the arranged children
		 */
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const = 0;

	private:
		/*
		 * explicitly set the desired size, this is highly advanced functionality that is meat
		 * to be used in conjuction with overriding cache desired size, use compute desired size() instead
		 */
		void set_desired_size(const glm::vec2& in_desired_size)
		{
			m_desired_size = in_desired_size;
		}

	public:
		/*
		 * @return is this widget visible, hidden or collapsed
		 * @note this widget can be visible but if a parent is hidden or collapsed, it would not show on screen
		 */
		EVisibility get_visibility() const { return m_Visibility_Attribute.Get(); }

		/*@param InVisibility should this widget is*/
		virtual void set_visibility(TAttribute<EVisibility> in_visibility);
		/*
		 * invalidates the widget from the view of a layout caching widget that may own this widget
		 * will force the owning widget to redraw and cache children on the next paint pass
		 */
		void Invalidate(EInvalidateWidgetReason in_validate_reason);

	protected:
		/*
		a slate attribute that is member variable of a SWidget
		usage : TSlateAttribute<int32_t> MyAttribute1; TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint> MyAttribute2; TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint, TSlateAttributeComparePredicate<>> MyAttribute3;
		*/
		template<typename InObjectType, EInvalidateWidgetReason InInvalidationReasonValue = EInvalidateWidgetReason::None, typename InComparePredicate = TSlateAttributeComparePredicate<>>
		struct TSlateAttribute : public SlateAttributePrivate::TSlateMemberAttribute<
			InObjectType,
			typename std::conditional<InInvalidationReasonValue == EInvalidateWidgetReason::None,
			SlateAttributePrivate::FSlateAttributeNoInvalidationReason,
			TSlateAttributeInvalidationReason<InInvalidationReasonValue>>::type,
			InComparePredicate>
		{
			using SlateAttributePrivate::TSlateMemberAttribute<
				InObjectType,
				typename std::conditional<InInvalidationReasonValue == EInvalidateWidgetReason::None,
				SlateAttributePrivate::FSlateAttributeNoInvalidationReason,
				TSlateAttributeInvalidationReason<InInvalidationReasonValue>>::type,
				InComparePredicate>::TSlateMemberAttribute;
		};

	public:
		/*
		* checks to see if this widget is the current mouse captor
		* 
		* @return true if this widget has captured the mouse
		*/
		bool has_mouse_capture() const;

		/*
		* checks to see if this widget has mouse capture from the provided user
		* 
		* @return true if this widget has captured the mouse
		*/
		bool has_mouse_capture_by_user(int32_t user_index, std::optional<int32_t> pointer_index = std::optional<int32_t>()) const;

		/*
		* tests if an arranged widget should be culled
		* 
		* @param MyCullingRect the culling rect of the widget currently doing the culling
		* @param ArrangedChild the arranged widget in the widget currently attempting to cull children
		*/
		bool is_child_widget_culled(const FSlateRect& my_culling_rect, const FArrangedWidget& arranged_children) const;
			 
	public:
		/* @return whether or not this widget is enabled */
		inline bool Is_Enabled() const
		{
			return m_enabled_state_attribute.Get();
		}

		/*
		 * @return true if this widget hovered
		 * @note IsHovered used to be virtual, use set hover to assign an attribute if you need to override the default behaviour
		 */
		bool is_hovered() const
		{
			return m_hovered_attribute.Get();
		}

		/*@return true if this widget is directly hovered*/
		bool is_directly_hovered() const;

	protected:
		/*
		 * set the hover state
		 * once set, the attribute that the ownership and SWidget code will not update the attribute value
		 * you can return the control to the SWidget code by setting an empty TAttribute
		 */
		bool set_hover(TAttribute<bool> in_hovered)
		{
			m_b_is_hovered_attribute_set = in_hovered.Is_Set();
			m_hovered_attribute.Assign(*this, std::move(in_hovered));
		}

	public:
		std::optional<FSlateRenderTransform> get_render_transform_with_respect_to_flow_direction() const
		{
			if(g_slate_flow_direction == EFlowDirection::LeftToRight)
			{
				return m_render_transform_attribute.Get();
			}
			else
			{
				//if we are going right to left, flip the x translation on render transforms
				std::optional<FSlateRenderTransform> transform = m_render_transform_attribute.Get();

				if(transform.has_value())
				{
					glm::vec2 translation = transform.value().get_translation();
					transform.value().set_translation(glm::vec2(-translation.x, translation.y));
				}
				return transform;
			}
		}

		glm::vec2 get_render_transform_pivot_with_respect_to_flow_direction() const
		{
			if(g_slate_flow_direction == EFlowDirection::LeftToRight)
			{
				return m_render_transform_pivot_attribute.Get();
			}
			else
			{
				//if we're going right to left, filp the x's pivot mirrored about 0.5
				glm::vec2 transform_pivot = m_render_transform_pivot_attribute.Get();
				transform_pivot.x = 0.5f + (0.5f - transform_pivot.x);
				return transform_pivot;
			}
		}

		/*
		 * gets the last geometry used to tick the widget, this data may not exist yet if this call happens prior to
		 * the widget having been ticked/painted, or it may be out of date, or a frame behind
		 */
		const FGeometry& get_paint_space_geometry() const;

	protected:
		/*the cursor to show when the mouse is hovering over this widget*/
		std::optional<EMouseCursor::Type> get_cursor() const;//note:from the meta data to get cursor type
	public:
		/*
		 * get the metadata of the type provided
		 * @return the first metadata of the type supplied that we encouter
		 */
		template<typename MetaDataType>
		std::shared_ptr<MetaDataType> get_meta_data() const
		{
			for(const auto& meta_data_entry : m_Meta_Data)
			{
				if(meta_data_entry->Is_Of_Type<MetaDataType>())
				{
					return std::static_pointer_cast<MetaDataType>(meta_data_entry);
				}
			}
			return std::shared_ptr<MetaDataType>();
		}

		/*
		 * add metadata to this widget
		 * @param AddMe the metadata to add to the widget
		 */
		template<typename MetaDataType>
		void add_meta_data(const std::shared_ptr<MetaDataType>& add_me)
		{
			add_meta_data_internal(add_me);
		}
	private:
		void add_meta_data_internal(const std::shared_ptr<ISlateMetaData>& add_me);
	public:
		/*see OnMouseMove event*/
		void set_on_mouse_move(FPointerEventHandler event_handler);


		/*@return the foreground color that this widget sets, unset options if the widget does not set a foreground color*/
		virtual FSlateColor get_foreground_color() const;

		/*@return the foreground color that this widget sets when this widget or any of its ancestors are disabled, unset options if the widget does not set a foreground color*/
		virtual FSlateColor get_disabled_foreground_color() const;

		/*
		* gets the last geometry used to tick the widget
		* this data may not exist yet if this call happens prior to the widget having been ticked/painted, or it may be out of date, or a frame behind
		*/
		const FGeometry& get_tick_space_geometry() const;
	public:
		/*
		 * hidden default constructor
		 *
		 * use SNew(WidgetClassName) to instantiate new widgets
		 *
		 * @see SNew
		 */
		SWidget();

		/*construct a SWidget based on initial parameters*/
		void SWidgetConstruct(const FSlateBaseNamedArgs& args);

		/*
		* find the geometry of descendant widget, this method assumes that widget to find is a descendant of this widget
		* 
		* @param MyGeometry the geometry of this widget
		* @param WidgetToFind the widget whose geometry we wish to discover
		* @return the geometry of widget to find
		*/
		FGeometry find_child_geometry(const FGeometry& my_geometry, std::shared_ptr<SWidget> widget_to_find) const;

		/*
		* find the geometry of a descendant widget. this method assumes that widgets to find are a descendants of this widget
		* note that not all widgets are guaranteed to be found, out results will contain null entries for missing widgets
		* 
		* @param MyGeometry the geometry of this widget
		* @param widgetstofind the widgets whose geometries we wish to discover
		* @param OutResult a map of widget references to their respective geometries
		* 
		* @return true if all the widget geometries were found, false otherwise
		*/
		bool find_child_geometries(const FGeometry& my_geometry, const std::set<std::shared_ptr<SWidget>>& widgets_to_find, std::map<std::shared_ptr<SWidget>, FArrangedWidget>& out_result) const;

		/*
		* actual implementation of find child geometries
		* 
		* @param my geometry the geometry of this widget
		* @param widgets to find the widgets whose geometries we wish to discover
		* @param out result a map of widget references to their respective geometries
		*/
		void find_child_geometries_helper(const FGeometry& my_geometry, const std::set<std::shared_ptr<SWidget>>& widgets_to_find, std::map<std::shared_ptr<SWidget>, FArrangedWidget>& out_result) const;
		/*
		 * called to tell a widget to paint itself (and it's children)
		 *
		 * the widget should respond by populating the out draw elements array with FDrawElements
		 * that represent it and any of it's children
		 *
		 * @param Args all the arguments necessary to paint this widget(@todo ump: move all params into this struct)
		 * @param AllottedGeometry the FGeometry that describes an area in which the widget should appear
		 * @param MyCullingRect the clipping rectangle allocated for this widget and it's children
		 * @param OutDrawElements a list of FDrawElements to populate with the output
		 * @param LayerId the layer onto which this widget should be rendered
		 * @param InColorAndOpacity color and opacity to be applied to all the descendants of the widget being paintes
		 * @param bParentEnabled true if the parent of this widget is enabled
		 * @return the maximum layer id attained by this widget or any of it's children
		 */
		int32_t paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const;

		/*
		* ticks this widget with geometry, override in derived classes, but always call the parent implementation
		* 
		* @param AllottedGeometry the space allotted for this widget
		* @param InCurrentTime current absolute real time
		* @param InDeltaTime real time passed since last tick
		*/
		virtual void Tick(const FGeometry& allotted_geometry, const double in_current_time, const float in_delta_time);

		virtual FReply On_Focus_Received(const FGeometry& my_geometry, const FFocusEvent& in_focus_event);

		/*
		* called after a character is entered while this widget has keyboard focus
		* 
		* @param MyGeometry the geometry of the widget receiving the event
		* 
		* @param InCharacterEvent character event
		* 
		* @return returns whether the event was handled, along with other possible actions
		*/
		virtual FReply On_Key_Char(const FGeometry& my_geometry, const FCharacterEvent& in_character_event);

		/*
		 * called after a key is pressed when this widget has focus(this event bubbles if not handled)
		 *
		 * @param MyGeometry the geometry of the widget receiving the event
		 * @param InKeyEvent key event
		 * @param returns whether the event was handled, along with other possible actions
		 */
		virtual FReply On_Key_Down(const FGeometry& my_geometry, const FKeyEvent& in_key_event);

		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event);//todo:add comment

		virtual FReply On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event);//todo:add comment

		/*
		* called when slate detects that a widget started to be dragged
		* 
		* usage:
		* a widget can ask slate to detect a drag
		* OnMouseDown() reply with FReply::Handled().DetectDrag(shared_this(this))
		* slate will either send a OnDragDected() event or do nothing
		* if the user releases a mouse button or leaves the widget before
		* a drag is triggered (maybe user started at the very edge) then no event will be
		* sent
		* 
		* @param InMyGeometry widget geometry
		* @param InMouseEvent mouse event that triggered the drag
		*/
		virtual FReply On_Drag_Detected(const FGeometry& my_geometry, const FPointerEvent& mouse_event);

		/*
		 *	called during drag and drop when the drag enters a widget
		 *
		 *	enter/leave events in slate are meant as lightweight notifications
		 *	so we do not want to capture mouse or set focus in response to these
		 *	however, OnDragEnter must also support external APIs (e.g. OLE Drag/Drop)
		 *	those requires that we let them know whether we can handle the content being dragged OnDragEnter
		 *
		 *	the concession is to return a can_handled/cannot_handle
		 *	boolean rather than a full FReply
		 *
		 *	@param MyGeometry the geometry of the widget receiving the event
		 *	@param DragDropEvent the drag and drop event
		 *
		 *	@return a reply that indicated whether the contents of the DragDropEvent can potentially be processed by this widget
		 */
		virtual void On_Drag_Enter(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event);

		/*
		* called during drag and drop when the drag leaves a widget
		* 
		* @param DragDropEvent the drag and drop event
		*/
		virtual void On_Drag_Leave(const FDragDropEvent& drag_drop_event);

		/*
		* called during drag and drop when the the mouse is being dragged over a widget
		* 
		* @param MyGeometry the geometry of the widget receiving the event
		* @param DragDropEvent the drag and drop event
		* @return a reply that indicated whether this event was handled
		*/
		virtual FReply On_Drag_Over(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event);

		/*
		* called when the user is dropping something onto a widget, terminates drag and drop
		* 
		* @param MyGeometry the geometry of the widget receiving the event
		* @param DragDropEvent the drag and drop event
		* @return a reply that indicated whether this event was handled
		*/
		virtual FReply On_Drop(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event);

		/*
		* the system asks each widget under the mouse to provide a cursor, this event is bubbled
		* 
		* @return FCursorReply::UnHandled() if the event is not handled, return FCursorReply::Cursor() otherwise
		*/
		virtual FCursorReply On_Cursor_Query(const FGeometry& my_geometry, const FPointerEvent& cursor_event) const;

		/*
		 * called when the mouse is moved over the widget's window, to determine if we should report whether
		 * OS-specific features should be active at this location (such as a title bar grip, system menu, etc.)
		 * usually you should not need to override this function
		 *
		 * @return the window "zone" the cursor is over, or EWindowZone::Unspecified if no special behavior is needed
		 */
		virtual EWindowZone::Type get_window_zone_override() const;

		void slate_prepass();

		/*
		 * descends to leaf-most widgets in the hierarchy and gathers desired sizes on the way up
		 * i.e. caches the desired size of all of this widget's children recursively, then caches desired size for itself
		 */
		void slate_prepass(float in_layout_scale_multiplier);

		/*
		 * the system calls this method to notify the widget that a mouse moved within it, this event is bubbled
		 *
		 * @param MyGeometry the geometry of the widget receiving the event
		 * @param MouseEvent information about the input event
		 * @return whether the event was handled along with possible requests for the system to take action
		 */
		virtual FReply On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event);

		/*
		* the system will use this event to notify a widget that the cursor has entered it, this event is uses a custom bubble strategy
		* 
		* @param MyGeometry the geometry of the widget receiving the event
		* @param MouseEvent information about the input event
		*/
		virtual void On_Mouse_Enter(const FGeometry& my_geometry, const FPointerEvent& mouse_event);

		/*
		* the system will use this event to notify a widget that the cursor has left it, this event is uses a custom bubble strategy
		* 
		* @param mouse event information about the input event
		*/
		virtual void On_Mouse_Leave(const FPointerEvent& mouse_event);

		/* is the widget construction completed(did we called and returned from the Construct() function) */
		bool Is_Constructed() const { return m_b_Is_Declarative_Syntax_Construction_Completed; }

		/*
		 * determines if this widget should be enabled
		 *
		 * @param InParentEnabled true if the parent of this widget is enabled
		 * @return true if the widget is enabled
		 */
		bool should_be_enabled(bool in_parent_enabled) const
		{
			//this widget should enabled if it's parent is enabled and it is enabled
			return in_parent_enabled && Is_Enabled();
		}

	private:
		/*
		 * the widget should respond by populating the OutDrawElements array with FDrawElements
		 * that represent it and any of it's children, called by the non-virtual OnPaint to enforce pre/post conditions
		 * during OnPaint
		 *
		 * @param Args all the arguments necessary to paint this widget(@todo umg:move all params into this struct)
		 * @param AllottedGeometry the FGeometry that describes an area in which the widget should appear
		 * @param MyCullingRect the rectangle representing the bounds currently being to completely cull widgets
		 * @param OutDrawElements a list of FDrawElements to populate with the output
		 * @param LayerId the layer onto which this widget should be rendered
		 * @param InColorAndOpacity color and opacity to be applied to all the descendants of the widget begin painted
		 * @param bParentEnabled true if the parent of this enabled
		 * @return the maximum layer id attained by this widget or any of it's children
		 */
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const = 0;

		void prepass_internal(float layout_scale_multipler);

	protected:

		void prepass_child_loop(float in_layout_scale_multiplier, FChildren* my_children);
	protected:
		/*
		 * can the widget ever support children? this will be false on SLeafWidgets
		 * rather than setting this directly, you should probably inherit from SLeafWidget
		 */
		uint8_t b_can_have_children : 1;

		uint8_t b_clipping_proxy : 1;//todo:how to understand it?

		/*
		* set to true if all content of the widget should clip to the bounds of this widget
		*/
		EWidgetClipping m_clipping;
	private:
		/* are bound slate attributes will be updated once per frame */
		uint8_t m_b_enabled_attributes_update : 1;

		/* the SNew or SAssignedNew construction is completed */
		uint8_t m_b_Is_Declarative_Syntax_Construction_Completed : 1;

		/*is the attribute IsHovered is set?*/
		uint8_t m_b_is_hovered_attribute_set : 1;

	private:

		mutable FSlateWidgetPersistentState m_persistent_state;

	protected:
		/*
		 * called when a child is removed from the tree parent's widget tree either by removing it from a slot
		 * this can also be called manually if you've got some non-slot based what of no longer reporting children
		 * an example of a widget that needs manual calling is SWidgetSwitcher
		 * it keeps all its children but only arranges and paints a single "active" one
		 * once a child becomes inactive, once a child becomes inactive, it's cached data should be removed
		 */
		void invalidate_child_remove_from_tree(SWidget& child);

	public:
		//return true if the widgets has any bound slate attribute
		bool Has_Registered_Slate_Attribute() const { return m_b_has_registered_slate_attribute; }

		/* pointer to this widgets parent widget. if it is null this is a root widget or it is not in the widget tree */
		std::weak_ptr<SWidget> m_parent_widget_ptr;

		/*the opacity of the widget, automatically applied during rendering*/
		float m_render_opacity;

		//todo:move this member to private control
		//meta data associated with this widget
		std::vector<std::shared_ptr<ISlateMetaData>> m_Meta_Data;
	private:
		/* stores the ideal size this widget wants to be */
		std::optional<glm::vec2> m_desired_size;

		/* is this widget visible, hidden or collapsed */
		TSlateAttribute<EVisibility> m_Visibility_Attribute;

		/* whether or not this widget is enabled */
		TSlateAttribute<bool> m_enabled_state_attribute;

		/* whether or not this widget is hovered*/
		TSlateAttribute<bool> m_hovered_attribute;

		/* render transform pivot of this widget(in normalized local space) */
		TSlateAttribute<glm::vec2> m_render_transform_pivot_attribute;

		/* render transform of this widget. TOptional<> to allow code to skip expensive overhead if there is no render transform applied */
		//TSlateAttribute<std::optional<FSlateRenderTransform>>
		//todo:add std::optional<>
		TSlateAttribute<FSlateRenderTransform> m_render_transform_attribute;

		//is there at least one slate attribute currently registered
		uint8_t m_b_has_registered_slate_attribute : 1;

	protected:

		FMargin m_culling_bounds_extension;//todo:how to understand it?

		std::optional<float> m_prepass_layout_scale_multiplier;
	};
}
