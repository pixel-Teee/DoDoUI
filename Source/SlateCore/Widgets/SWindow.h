#pragma once

#include "SCompoundWidget.h"

#include "ApplicationCore/GenericPlatform/GenericWindowDefinition.h"

#include "SlateCore/FastUpdate/SlateInvalidationRoot.h"//FSlateInvalidationContext

namespace DoDo
{
	class Window;//native window
	//todo:let SWindow to inherited from FSlateInvalidationRoot
	class SWindow : public SCompoundWidget, public FSlateInvalidationRoot
	{
	public:
		SLATE_BEGIN_ARGS(SWindow)
			: _Type(EWindowType::Normal)
			, _Title()
			, _ScreenPosition(glm::vec2(0.0f, 0.0f))
			, _ClientSize(glm::vec2(0.0f, 0.0f))
			, _AdjustInitialSizeAndPositionForDPIScale(true)
		{}
			/*type of this window*/
			SLATE_ARGUMENT(EWindowType, Type)
			/*title of the window*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Title)
			/*screen-space position where the window should be initially located*/
			SLATE_ARGUMENT(glm::vec2, ScreenPosition)
			/*what the initial size of the window should be*/
			SLATE_ARGUMENT(glm::vec2, ClientSize)
			/*if the initial ClientSize and ScreenPosition arguments should be automatically adjusted to account for DPI scale*/
			SLATE_ARGUMENT(bool, AdjustInitialSizeAndPositionForDPIScale)

			SLATE_DEFAULT_SLOT(FArguments, Content)//declare a slot

		SLATE_END_ARGS()

		//SWindow();

		//todo:implement FSlateWindowElementList FWidgetStyle 
		//int32_t paint_window(double current_time, float delta_time);

		virtual ~SWindow() {}

	public:
		void Construct(const FArguments& in_args);

		/*return true if the window is visible, false otherwise*/
		bool is_visible() const;

		/*
		* grabs the window type
		* 
		* @return the window's type
		*/
		EWindowType get_type() const
		{
			return m_type;
		}

		/*
		* grabs the current window title
		* 
		* @return the window's title
		*/
		DoDoUtf8String get_title() const
		{
			return m_title.Get();
		}

		/*@return the geometry of the window in window space(i.e. position and absolute position are 0)*/
		FGeometry get_window_geometry_in_window() const;

		/*paint the window and all of it's contents, not the same as Paint()*/
		int32_t paint_window(double current_time, float delta_time, FSlateWindowElementList& out_draw_elements, const FWidgetStyle& in_widget_style, bool b_parent_enabled);//todo:implement FWidgetStyle

		std::shared_ptr<Window> get_native_window();

		/*return the parent of this window, invalid shared pointer if this window is not a child*/
		std::shared_ptr<SWindow> get_parent_window() const;

		/*
		 * returns the viewport size, taking into consideration if the window size should drive the viewport size
		 */
		glm::vec2 get_view_port_size() const
		{
			return (m_view_port_size.x) ? m_view_port_size : m_size;
		}

		/*
		* @return the initially desired screen position of the slate window
		*/
		glm::vec2 get_initial_desired_size_in_screen() const;

		/*
		* @return the initially desired size of the slate window
		*/
		glm::vec2 get_initial_desired_position_in_screen() const;

		/*
		 * @return the size of the window in screen space
		 */
		glm::vec2 get_size_in_screen() const;

		/*make the window visible*/
		void show_window();

		/*
		* sets the native os window associated with this SWindow
		* 
		* @param InNativeWindow the native window
		*/
		void set_native_window(std::shared_ptr<Window> in_native_window);

		/*sets the actual screen position of the window, this should only be called by the os*/
		//note:called by the os
		void set_cached_size(glm::vec2 new_size);

		/*
		 * sets the widget content for this window
		 *
		 * @param InContent the widget to use as content for this window
		 */
		void set_content(std::shared_ptr<SWidget> in_content);

		/*resize using already dpi scaled window size including borders/title bar*/
		void resize_window_size(glm::vec2 new_window_size);
	private:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
	protected:
		/*type of the window*/
		EWindowType m_type;

		/*title of the window, displayed in the title bar as well as potentially in the task bar (windows platform)*/
		TAttribute<DoDoUtf8String> m_title;

		/*initial desired position of the window's content in screen space*/
		glm::vec2 m_initial_desired_screen_position;

		/*initial desired position of the window's content in screen space*/
		glm::vec2 m_initial_desired_size;

		/*position of the window's content in screen position*/
		glm::vec2 m_screen_position;

		/*size of the window's content area in screen space*/
		glm::vec2 m_size;

		/*size of the viewport, if (0, 0) then it is equal to size*/
		glm::vec2 m_view_port_size;

	protected:
		/*the native window that is backing this slate window*/
		//todo:implement FGenericWindow
		std::shared_ptr<Window> m_native_window;

		/*when not null, this window will always appear on top of the parent and be closed when the parent is closed*/
		std::weak_ptr<SWindow> m_parent_window_ptr;

	protected:

		//------FSlateInvalidationRoot overrides------
		virtual int32_t paint_slow_path(const FSlateInvalidationContext& context) override;
		//------FSlateInvalidationRoot overrides------
	};
}
