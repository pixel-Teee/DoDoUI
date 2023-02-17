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

namespace DoDo {
	//SWindow::SWindow()
	//{
	//}

	void DoDo::SWindow::set_content(std::shared_ptr<SWidget> in_content)
	{
		m_child_slot.operator[](in_content);

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
		, m_hittest_grid(std::make_unique<FHittestGrid>())
	{
	}

	void SWindow::Construct(const FArguments& in_args)
	{
		this->m_type = in_args._Type;
		this->m_title = in_args._Title;

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
		FSlateLayoutTransform local_to_window(1.0f);//todo:implement GetLocalToWindowTransform

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

	glm::vec2 SWindow::get_size_in_screen() const
	{
		return m_size;//window size
	}

	glm::vec2 SWindow::get_position_in_screen() const
	{
		return m_screen_position;//window position
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

	int32_t SWindow::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		int32_t max_layer = SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);

		return max_layer;
	}

	int32_t SWindow::paint_slow_path(const FSlateInvalidationContext& invalidation_context)
	{
		//todo:clear hittest grid

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

}