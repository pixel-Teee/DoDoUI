#pragma once

#include "SCompoundWidget.h"

#include "ApplicationCore/GenericPlatform/GenericWindowDefinition.h"

namespace DoDo
{
	class Window;//native window
	class SWindow : public SCompoundWidget
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

		std::shared_ptr<Window> get_native_window();

		/*return the parent of this window, invalid shared pointer if this window is not a child*/
		std::shared_ptr<SWindow> get_parent_window() const;

		/*
		* @return the initially desired screen position of the slate window
		*/
		glm::vec2 get_initial_desired_size_in_screen() const;

		/*
		* @return the initially desired size of the slate window
		*/
		glm::vec2 get_initial_desired_position_in_screen() const;

		/*
		* sets the native os window associated with this SWindow
		* 
		* @param InNativeWindow the native window
		*/
		void set_native_window(std::shared_ptr<Window> in_native_window);

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
	};
}