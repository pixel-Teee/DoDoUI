#include <PreCompileHeader.h>

#include "SWindow.h"

#include "Application/Application.h"//slate application
#include "Core/Window.h"
#include "Renderer/Renderer.h"
#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"
#include "SlateCore/Layout/BasicLayoutWidgetSlot.h"
#include "SlateCore/Layout/Geometry.h"

#ifdef WIN32
#include "Platform/Application/WindowsPlatformApplicationMisc.h"
#else
#include "Platform/Application/AndroidPlatformApplicationMisc.h"
#endif

#include "SlateCore/Types/PaintArgs.h"

#include "SlateCore/Application/SlateApplicationBase.h"//FWindowTitleBarArgs depends on it

#include "SlateCore/Widgets/SOverlay.h"//SOverlay depends on it

#include "SlateCore/Widgets/Images/SImage.h"//SImage depends on it

namespace DoDo {
	//SWindow::SWindow()
	//{
	//}

	void DoDo::SWindow::set_content(std::shared_ptr<SWidget> in_content)
	{
		//m_child_slot.operator[](in_content);

		if (m_content_slot)
		{
			m_content_slot->operator[](in_content);
		}
		else
		{
			m_child_slot.operator[](in_content);
		}

		//todo:call Invalidate, child order
	}

	bool SWindow::is_screen_space_mouse_within(glm::vec2 screen_space_mouse_coordinate) const
	{
		//const glm::vec2 local_mouse_coordinate = screen_space_mouse_coordinate - m_screen_position;//get the local mouse position
		const glm::vec2 local_mouse_coordinate = screen_space_mouse_coordinate - m_screen_position;

		return m_native_window->is_point_in_window(local_mouse_coordinate.x, local_mouse_coordinate.y);//check mouse in the native window
	}

	SWindow::SWindow()
		: m_initial_desired_screen_position(glm::vec2(0.0f))
		, m_initial_desired_size(glm::vec2(0.0f, 0.0f))
		, m_size(glm::vec2(0.0f, 0.0f))
		, m_view_port_size(glm::vec2(0.0f, 0.0f))
		, m_title_bar_size(SWindowDefs::default_title_bar_size)
		, m_content_slot(nullptr)
		, m_hittest_grid(std::make_unique<FHittestGrid>())
	{
	}

	void SWindow::Construct(const FArguments& in_args)
	{
		this->m_type = in_args._Type;
		this->m_title = in_args._Title;
		this->m_style = in_args._Style;
		this->m_window_background = &in_args._Style->m_background_brush;
		this->m_user_resize_border = in_args._UserResizeBorder;

		//calculate window size from client size
		b_create_title_bar = in_args._CreateTitleBar;//todo:add other check

		//calculate initial window position
		glm::vec2 window_position = in_args._ScreenPosition;

		if (in_args._AdjustInitialSizeAndPositionForDPIScale && window_position != glm::vec2(0.0f, 0.0f))
		{
			//will need to add additional logic to walk over multiple montiors at various DPIs to determine correct window position
			const float initial_dpi_scale = FPlatformApplicationMisc::get_dpi_scale_factor_at_point(window_position.x, window_position.y);
			window_position *= initial_dpi_scale;
		}

		float dpi_scale = 1.0f;
		dpi_scale = FPlatformApplicationMisc::get_dpi_scale_factor_at_point(window_position.x, window_position.y);

		const glm::vec2 dpi_scaled_client_size = in_args._AdjustInitialSizeAndPositionForDPIScale ? in_args._ClientSize * dpi_scale : in_args._ClientSize;

		this->m_initial_desired_screen_position = window_position;
		this->m_initial_desired_size = dpi_scaled_client_size;//todo:need to interms of the dpi to scale

		resize_window_size(m_initial_desired_size);

		this->construct_window_internals();

		this->set_content(in_args._Content.m_widget);//set content, move FArgument's widget to this function
	}

	bool SWindow::is_visible() const
	{
		return true;
	}

	FGeometry SWindow::get_window_geometry_in_window() const
	{
		//note:this is root of the window hierarchy

		//we are scaling children for layout, but our pixel bounds are not changing
		//FGeometry expects size in local space, but our size is stored in screen space(same as window space + screen offset)
		//so we need to transform size into the window's local space for FGeometry
		FSlateLayoutTransform local_to_window;//todo:implement GetLocalToWindowTransform

		glm::vec2 view_size = get_view_port_size();//note:if viewport size is 0, then return size

		//note:if we want amplify viewport, we need to get a inverse scale layout transform, transform vector will do noting for translation, will skip this translation
		return FGeometry::make_root(transform_vector(inverse(local_to_window), view_size), local_to_window);
	}

	int32_t SWindow::paint_window(double current_time, float delta_time, FSlateWindowElementList& out_draw_elements, const FWidgetStyle& in_widget_style,
	                              bool b_parent_enabled)
	{
		//todo:clear hittest grid
		const bool hittest_cleared = m_hittest_grid->Set_Hittest_Area(get_position_in_screen(), get_view_port_size());

		//------construct paint args------
		FPaintArgs paint_args(nullptr, get_hittest_grid(), get_position_in_screen(), current_time, delta_time);
		//------construct paint args------

		FSlateInvalidationContext context(out_draw_elements, in_widget_style);

		context.m_paint_args = &paint_args;//get the paint args, pass to context class
		context.m_in_coming_layer_id = 0;

		m_persistent_state.m_allotted_geometry = get_window_geometry_in_window();
		//todo:implement culling bounds

		FSlateInvalidationResult result = paint_invalidation_root(context);//call FSlateInvalidationRoot's function, to slow path

		return result.m_max_layer_id_painted;
	}

	std::shared_ptr<Window> SWindow::get_native_window()
	{
		return m_native_window;//holds the life time
	}

	const std::vector<std::shared_ptr<SWindow>>& SWindow::get_child_windows() const
	{
		return m_child_windows;
	}

	std::vector<std::shared_ptr<SWindow>>& SWindow::get_child_windows()
	{
		return m_child_windows;
	}

	std::shared_ptr<SWindow> SWindow::get_parent_window() const
	{
		return m_parent_window_ptr.lock();//promote
	}

	FHittestGrid& SWindow::get_hittest_grid()
	{
		return *m_hittest_grid;
	}

	glm::vec2 SWindow::get_initial_desired_size_in_screen() const
	{
		return m_initial_desired_size;
	}

	glm::vec2 SWindow::get_initial_desired_position_in_screen() const
	{
		return m_initial_desired_screen_position;
	}

	glm::vec2 SWindow::get_desired_size_desktop_pixels() const
	{
		return this->get_desired_size();
	}

	glm::vec2 SWindow::get_size_in_screen() const
	{
		return m_size;//window size
	}

	glm::vec2 SWindow::get_position_in_screen() const
	{
		return m_screen_position;//window position
	}

	FSlateRect SWindow::get_clipping_rectangle_in_window() const
	{
		glm::vec2 view_size = get_view_port_size();

		return FSlateRect(0.0f, 0.0f, view_size.x, view_size.y);
	}

	void SWindow::request_destroy_window()
	{
		//todo:add request destroy window override

		Application::get().request_destroy_window(std::static_pointer_cast<SWindow>(shared_from_this()));
	}

	void SWindow::destroy_window_immediately()
	{
		if (m_native_window)
		{
			//destroy the native window
			m_native_window->destroy();
		}
	}

	void SWindow::show_window()
	{
		if(m_native_window)
		{
			//we can only create a viewport after the window has been shown(otherwise the swap chain creation may fail)
			Application::get().get_renderer()->create_view_port(std::static_pointer_cast<SWindow>(shared_from_this()));
		}

		//reshape window, to set screen position and size, todo:fix me, don't use initial desired size
		reshape_window(m_initial_desired_screen_position, m_initial_desired_size);
	}

	void SWindow::set_native_window(std::shared_ptr<Window> in_native_window)
	{
		m_native_window = in_native_window;//os related window
	}

	void SWindow::set_cached_screen_position(glm::vec2 new_position)
	{
		m_screen_position = new_position;

		//todo:implement invalidate screen position

		//todo:call on window moved delegate
	}

	void SWindow::set_cached_size(glm::vec2 new_size)
	{
		if(m_native_window)
		{
			//todo:implement adjust cached size
		}

		if(m_size != new_size)
		{
			m_size = new_size;

			//todo:implement invalidate root child order
		}
	}

	void SWindow::move_window_to(glm::vec2 new_position)
	{
		if (m_native_window)
		{
			glm::vec2 speculative_screen_position(new_position.x, new_position.y);
			set_cached_screen_position(speculative_screen_position);//note:update window position information

			//std::cout << "new window pos" << new_position.x << " " << new_position.y << std::endl;
			m_native_window->move_window_to(new_position.x, new_position.y);
		}
		else
		{
			m_initial_desired_screen_position = new_position;
		}
	}

	void SWindow::reshape_window(glm::vec2 new_position, glm::vec2 new_size)
	{
		const glm::vec2 current_position = get_position_in_screen();
		const glm::vec2 current_size = get_size_in_screen();

		//todo:check difference between parameter
		if(m_native_window)
		{
			set_cached_screen_position(new_position);

			//todo:implement native window reshape window

		}

		set_cached_size(new_size);
	}

	EWindowZone::Type SWindow::get_current_window_zone(glm::vec2 local_mouse_position)
	{
		//todo:check is full screen

		//todo:check is game window and don't have os window border

		//todo:get dpi scale
		const float window_dpi_scale = 1.0f;

		//todo:implement dpi scale resize border

		const FMargin dpi_scaled_resize_border = m_user_resize_border * window_dpi_scale;

		const bool b_is_cursor_visible = Application::get().get_platform_cursor()->get_type() != EMouseCursor::None;

		//don't allow position/resizing of window while in fullscreen mode by ignoring title bar/border zones
		//todo:add check

		if(local_mouse_position.x >= 0 && local_mouse_position.x < m_size.x &&
			local_mouse_position.y >= 0 && local_mouse_position.y < m_size.y)
		{
			int32_t row = 1;
			int32_t col = 1;

			//todo:add sizing rule check
			if(local_mouse_position.x < (dpi_scaled_resize_border.left + 5))
			{
				col = 0;
			}
			else if(local_mouse_position.x >= m_size.x - (dpi_scaled_resize_border.right + 5))
			{
				col = 2;
			}

			if(local_mouse_position.y < (dpi_scaled_resize_border.top + 5))
			{
				row = 0;
			}
			else if(local_mouse_position.y >= m_size.y - (dpi_scaled_resize_border.bottom + 5))
			{
				row = 2;
			}

			//the actual border is smaller than the hit result zones
			//this grants larger corner areas to grab onto
			bool b_in_border = local_mouse_position.x < dpi_scaled_resize_border.left ||
				local_mouse_position.x >= m_size.x - dpi_scaled_resize_border.right ||
				local_mouse_position.y < dpi_scaled_resize_border.top ||
				local_mouse_position.y >= m_size.y - dpi_scaled_resize_border.bottom;

			if(!b_in_border)
			{
				row = 1;
				col = 1;
			}

			static const EWindowZone::Type type_zones[3][3] =
			{
				{EWindowZone::TopLeftBorder, EWindowZone::TopBorder, EWindowZone::TopRightBorder},
				{EWindowZone::LeftBorder, EWindowZone::ClientArea, EWindowZone::RightBorder},
				{EWindowZone::BottomLeftBorder, EWindowZone::BottomBorder, EWindowZone::BottomRightBorder}
			};

			EWindowZone::Type in_zone = type_zones[row][col];

			if(in_zone == EWindowZone::ClientArea)
			{
				//hittest to see if the widget under the mouse should be treated as a title bar (i.e. should move the window)
				FWidgetPath hit_test_results = Application::get().get_hit_testing().locate_widget_in_window(Application::get().get_cursor_pos(),
					std::static_pointer_cast<SWindow>(shared_from_this()), false, -1);

				if(hit_test_results.m_widgets.num() > 0)
				{
					const EWindowZone::Type zone_override = hit_test_results.m_widgets.last().m_widget->get_window_zone_override();

					if(zone_override != EWindowZone::Unspecified)
					{
						//the widget override the window zone
						in_zone = zone_override;
					}
					else if(hit_test_results.m_widgets.last().m_widget == shared_from_this())
					{
						//the window itself was hit, so check for a traditional title bar
						if((local_mouse_position.y - dpi_scaled_resize_border.top) < SWindowDefs::default_title_bar_size * window_dpi_scale)
						{
							in_zone = EWindowZone::TitleBar;
						}
					}
				}

				m_window_zone = in_zone;
			}
			else
			{
				m_window_zone = in_zone;
			}
		}
		else
		{
			m_window_zone = EWindowZone::NotInWindow;
		}

		return m_window_zone;
	}

	int32_t SWindow::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		int32_t max_layer = SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);

		return max_layer;
	}

	void SWindow::construct_window_internals()
	{
		//set up widget that represents the main area of the window, that is, everything inside the window's border

		std::shared_ptr<SVerticalBox> main_window_area = SNew(SVerticalBox);

		FWindowTitleBarArgs args(std::static_pointer_cast<SWindow>(shared_from_this()));

		//todo:create window title bar
		std::shared_ptr<SWidget> title_bar_widget = Application::get().make_window_title_bar(args, m_title_bar);

		//create title bar
		if (b_create_title_bar)
		{
			m_title_bar_size = SWindowDefs::default_title_bar_size;
			main_window_area->add_slot()
			.auto_height()
			[
				title_bar_widget
			];//todo:fix me, may be conflict with SDockArea
		}
		else
		{
			m_title_bar_size = 0.0f;
		}
		

		//create window
		m_window_background_image =
			Application::get().make_image(
				m_window_background,
				m_style->m_background_color,
				EVisibility::visible
			);
		//create window content slot
		main_window_area->add_slot()
			.fill_height(1.0f)
			.Expose(m_content_slot)
			[
				SNullWidget::NullWidget
			];

		this->m_child_slot
		[
			SAssignNew(m_window_overlay, SOverlay)
			//window background
			+ SOverlay::Slot()
			[
				m_window_background_image
			]
			//main area
			+ SOverlay::Slot()
			[
				SAssignNew(m_content_area_v_box, SVerticalBox)
				+ SVerticalBox::Slot()
				[
					main_window_area
				]
			]
		];
	}

	int32_t SWindow::paint_slow_path(const FSlateInvalidationContext& invalidation_context)
	{
		//todo:clear hittest grid
		m_hittest_grid->clear();//note:this is important, because SDockTab switch need to clear hittest grid

		const FSlateRect window_culling_bounds;
		const int32_t layer_id = 0;

		//todo:get geometry
		FGeometry window_geometry = get_window_geometry_in_window();

		int32_t max_layer_id = 0;

		max_layer_id = paint(*invalidation_context.m_paint_args, window_geometry, window_culling_bounds, *invalidation_context.m_window_element_list, layer_id, invalidation_context.m_widget_style,
			invalidation_context.m_b_parent_enabled);

		//max_layer_id = Pain
		
		return max_layer_id;
	}

	void SWindow::resize_window_size(glm::vec2 new_window_size)
	{
		//get current size and new size to check equal

		if (m_size != new_window_size)
		{
			//resize window
			if (m_native_window)
			{
				//call native window's reshape window
			}
			else
			{
				m_initial_desired_size = new_window_size;
			}
		}

		//todo:implement set cached size
		set_cached_size(new_window_size);
	}

	FReply SWindow::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if(mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
		{
			m_move_resize_zone = m_window_zone;
			return FReply::handled().capture_mouse(shared_from_this());
		}
		else
		{
			return FReply::un_handled();
		}
	}

	FReply SWindow::On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if(this->has_mouse_capture() && mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
		{
			m_move_resize_zone = EWindowZone::Unspecified;
			return FReply::handled().release_mouse_capture();
		}
		else
		{
			return FReply::un_handled();
		}
	}

	FReply SWindow::On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if(this->has_mouse_capture() && mouse_event.is_mouse_button_down(EKeys::LeftMouseButton) && m_move_resize_zone == EWindowZone::TitleBar) //todo:fix me
		{
			this->move_window_to(m_screen_position + mouse_event.get_cursor_delta());

			//std::cout << "screen position:(" << m_screen_position.x << ", " << m_screen_position.y << ")" << std::endl;

			return FReply::handled();
		}
		else
		{
			//std::cout << "xx" << std::endl;
			return FReply::un_handled();
		}
	}
}
