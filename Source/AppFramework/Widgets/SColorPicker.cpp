#include "PreCompileHeader.h"

#include "SColorPicker.h"

#include "SlateCore/Widgets/SWindow.h"//color picker window depends on it

#include "Application/Application.h"//get_cursor_pos depends on it

#include "Slate/Widgets/Layout/SBorder.h"//SBorder depends on it

namespace DoDo {
	/*a default window size for the color picker which looks nice*/
	const glm::vec2 SColorPicker::default_window_size = glm::vec2(441, 537);
	/*
	* a static color picker that everything should use
	*/
	static std::weak_ptr<SWindow> color_picker_window;

	static std::weak_ptr<SColorPicker> global_color_picker;

	SColorPicker::~SColorPicker()
	{
	}
	void SColorPicker::Construct(const FArguments& in_args)
	{
		m_target_color_attribute = in_args._TargetColorAttribute;
		m_current_color_hsv = m_old_color = m_target_color_attribute.Get().linear_rgb_to_hsv();//h u v
	}
	bool open_color_picker(const FColorPickerArgs& args)
	{
		destroy_color_picker();

		bool result = false;

		FLinearColor old_color = args.m_initial_color_override;//initial color

		if (args.m_color_array && args.m_color_array->size() > 0)
		{
			old_color = FLinearColor(*(*args.m_color_array)[0]);
		}
		else if (args.m_linear_color_array && args.m_linear_color_array->size() > 0)
		{
			old_color = *(*args.m_linear_color_array)[0];
		}
		//todo:check on color committed is bound?

		//determine the position of the window so that it will spawn near the mouse, but not go off the screen
		glm::vec2 cursor_pos = Application::get().get_cursor_pos();

		FSlateRect anchor(cursor_pos.x, cursor_pos.y, cursor_pos.x, cursor_pos.y);

		glm::vec2 adjusted_summon_location = Application::get().calculate_popup_window_position(anchor, SColorPicker::default_window_size, true, glm::vec2(0.0f), Orient_Horizontal);

		//only override the color picker window creation behavior if we are not creating a modal color picker
		const bool b_override_non_modal_creation = true;//todo:check modal

		std::shared_ptr<SWindow> window = nullptr;
		std::shared_ptr<SBorder> window_content = SNew(SBorder)
						.BorderImage(FAppStyle::get().get_brush("Brushes.Panel"))
						.Padding(FMargin(8.0f, 8.0f));

		bool b_need_to_add_window = true;

		//todo:add modal create

		std::shared_ptr<SColorPicker> created_color_picker = SNew(SColorPicker)
			.TargetColorAttribute(old_color)
			.TargetFColors(args.m_color_array ? *args.m_color_array : std::vector<FColor*>())
			.TargetLinearColors(args.m_linear_color_array ? *args.m_linear_color_array : std::vector<FLinearColor*>())
			.UseAlpha(args.b_user_alpha)
			.ParentWindow(window);
		
		if (b_override_non_modal_creation)
		{
			result = true;

			//hold on to the window created for external use...
			color_picker_window = window;
		}

		global_color_picker = created_color_picker;
		return result;
	}
	void destroy_color_picker()
	{
		if (color_picker_window.lock())
		{
			//todo:add on color picker destroy override

			color_picker_window.lock()->request_destroy_window();

			color_picker_window.reset();
			global_color_picker.reset();
		}
	}
}