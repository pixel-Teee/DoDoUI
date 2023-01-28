#include <PreCompileHeader.h>

#include "SWindow.h"

#ifdef WIN32
#include "Platform/Application/WindowsPlatformApplicationMisc.h"
#endif

namespace DoDo {
	//SWindow::SWindow()
	//{
	//}

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
	}

	bool SWindow::is_visible() const
	{
		return true;
	}

	std::shared_ptr<Window> SWindow::get_native_window()
	{
		return m_native_window;//holds the life time
	}

	std::shared_ptr<SWindow> SWindow::get_parent_window() const
	{
		return m_parent_window_ptr.lock();//promote
	}

	glm::vec2 SWindow::get_initial_desired_size_in_screen() const
	{
		return m_initial_desired_size;
	}

	glm::vec2 SWindow::get_initial_desired_position_in_screen() const
	{
		return m_initial_desired_screen_position;
	}

	void SWindow::set_native_window(std::shared_ptr<Window> in_native_window)
	{
		m_native_window = in_native_window;//os related window
	}

	int32_t SWindow::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		int32_t max_layer = SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);

		return max_layer;
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
	}

}