#include <PreCompileHeader.h>

#include "SWindow.h"

namespace DoDo {
	//SWindow::SWindow()
	//{
	//}

	void SWindow::Construct(const FArguments& in_args)
	{
		this->m_type = in_args._Type;
		this->m_title = in_args._Title;
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

}