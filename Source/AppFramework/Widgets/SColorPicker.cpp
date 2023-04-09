#include "PreCompileHeader.h"

#include "SColorPicker.h"

#include "SlateCore/Widgets/SWindow.h"//color picker window depends on it

#include "Application/Application.h"//get_cursor_pos depends on it

#include "Slate/Widgets/Layout/SBorder.h"//SBorder depends on it

#include "Slate/Widgets/Colors/SColorWheel.h"//SColorWheel depends on it

#include "SlateCore/Widgets/SOverlay.h"//SOverlay depends on it

#include "Slate/Widgets/Input/SSlider.h"//SSlider depends on it

#include "Slate/Widgets/Colors/SSimpleGradient.h"//SSimpleGradient depends on it

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

		m_b_is_interactive = false;

		generate_inline_color_picker_content();//inline version
	}
	void SColorPicker::handle_color_spectrum_value_changed(FLinearColor new_value)
	{
		set_new_target_color_hsv(new_value);
	}
	bool SColorPicker::set_new_target_color_hsv(const FLinearColor& new_value, bool b_force_update)
	{
		m_current_color_hsv = new_value;
		m_current_color_rgb = new_value.hsv_to_linear_rgb();

		return apply_new_target_color(b_force_update);
	}
	bool SColorPicker::set_new_target_color_rgb(const FLinearColor& new_value, bool b_force_update)
	{
		m_current_color_rgb = new_value;
		m_current_color_hsv = new_value.linear_rgb_to_hsv();

		return apply_new_target_color(b_force_update);
	}
	bool SColorPicker::apply_new_target_color(bool b_force_update)
	{
		bool b_update = false;

		if (b_force_update) //todo:add more parameter check
		{
			update_color_pick_mouse_up();

			b_update = true;
		}

		return b_update;
	}
	void SColorPicker::update_color_pick()
	{
		FLinearColor out_color = m_current_color_rgb;//note:current color picked in rgb

		set_colors(out_color);

		//todo:on color committed execute
	}
	void SColorPicker::update_color_pick_mouse_up()
	{
		update_color_pick();
	}
	void SColorPicker::generate_inline_color_picker_content()
	{
		std::shared_ptr<SWidget> alpha_slider = SNullWidget::NullWidget;

		m_child_slot
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.fill_width(1.0f)
			.HAlign(HAlign_Center)
			[
				SNew(SColorWheel)
				.SelectedColor(this, &SColorPicker::get_current_color)
				//todo:add visibility bind
				.OnValueChanged(this, &SColorPicker::handle_color_spectrum_value_changed)
				.OnMouseCaptureBegin(this, &SColorPicker::handle_interactive_change_begin)
				.OnMouseCaptureEnd(this, &SColorPicker::handle_interactive_change_end)
			]
			+ SHorizontalBox::Slot()
			.auto_width()
			.Padding(4.0f, 0.0f)
			[
				//saturation slider
				make_color_slider(EColorPickerChannels::Saturation)
			]
			+ SHorizontalBox::Slot()
			.auto_width()
			[
				//value slider
				make_color_slider(EColorPickerChannels::Value)
			]
			//+ SHorizontalBox::Slot()
			//.auto_width()
			//[
			//	//alpha slider
			//	make_color_slider(EColorPickerChannels::Alpha)
			//]
		];
	}
	FLinearColor SColorPicker::handle_color_slider_end_color(EColorPickerChannels channel) const
	{
		switch (channel)
		{
		case EColorPickerChannels::Red: return FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		case EColorPickerChannels::Green: return FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		case EColorPickerChannels::Blue: return FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
		case EColorPickerChannels::Alpha: return FLinearColor(m_current_color_rgb.R, m_current_color_rgb.G, m_current_color_rgb.B, 0.0f);
		case EColorPickerChannels::Saturation: return FLinearColor(m_current_color_hsv.R, 0.0f, 1.0f, 1.0f).hsv_to_linear_rgb();
		case EColorPickerChannels::Value: return FLinearColor(m_current_color_hsv.R, m_current_color_hsv.G, 0.0, 1.0f).hsv_to_linear_rgb();
		default: return FLinearColor();
		}
	}
	FLinearColor SColorPicker::handle_color_slider_start_color(EColorPickerChannels channel) const
	{
		switch (channel)
		{
		case EColorPickerChannels::Red: return FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
		case EColorPickerChannels::Green: return FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
		case EColorPickerChannels::Blue: return FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);
		case EColorPickerChannels::Alpha: return FLinearColor(m_current_color_rgb.R, m_current_color_rgb.G, m_current_color_rgb.B, 0.0f);
		case EColorPickerChannels::Saturation: return FLinearColor(m_current_color_hsv.R, 1.0f, 1.0f, 1.0f).hsv_to_linear_rgb();
		case EColorPickerChannels::Value: return FLinearColor(m_current_color_hsv.R, m_current_color_hsv.G, 1.0, 1.0f).hsv_to_linear_rgb();
		default: return FLinearColor();
		}
	}
	void SColorPicker::handle_interactive_change_begin()
	{
		m_b_is_interactive = true;
	}
	void SColorPicker::handle_interactive_change_end()
	{
		handle_interactive_change_end(0.0f);
	}
	void SColorPicker::handle_interactive_change_end(float new_value)
	{
		m_b_is_interactive = false;


	}
	void SColorPicker::handle_color_spin_box_value_changed(float new_value, EColorPickerChannels channel)
	{
		int32_t component_index;
		bool is_hsv = false;

		switch (channel)
		{
		case DoDo::EColorPickerChannels::Red: component_index = 0; break;
		case DoDo::EColorPickerChannels::Green: component_index = 1; break;
		case DoDo::EColorPickerChannels::Blue: component_index = 2; break;
		case DoDo::EColorPickerChannels::Alpha: component_index = 3; break;
		case DoDo::EColorPickerChannels::Hue: component_index = 0; is_hsv = true; break;
		case DoDo::EColorPickerChannels::Saturation: component_index = 1; is_hsv = true; break;
		case DoDo::EColorPickerChannels::Value: component_index = 2; is_hsv = true; break;
		default:
			return;
		}

		FLinearColor& new_color = is_hsv ? m_current_color_hsv : m_current_color_rgb;

		//todo:check zero

		new_color.component(component_index) = new_value;//assign new value

		if (is_hsv)
		{
			set_new_target_color_hsv(new_color, !m_b_is_interactive);//when interative end, then update color
		}
		else
		{
			set_new_target_color_rgb(new_color, !m_b_is_interactive);
		}
	}
	std::shared_ptr<SWidget> SColorPicker::make_color_slider(EColorPickerChannels channel) const
	{
		DoDoUtf8String slider_tool_tip;

		switch (channel)
		{
		case EColorPickerChannels::Red: slider_tool_tip = "Red"; break;
		case EColorPickerChannels::Green: slider_tool_tip = "Green"; break;
		case EColorPickerChannels::Blue: slider_tool_tip = "Blue"; break;
		case EColorPickerChannels::Alpha: slider_tool_tip = "Alpha"; break;
		case EColorPickerChannels::Hue: slider_tool_tip = "Hue"; break;
		case EColorPickerChannels::Saturation: slider_tool_tip = "Saturation"; break;
		case EColorPickerChannels::Value: slider_tool_tip = "Value"; break;
		default:
			return SNullWidget::NullWidget;
		}
		
		return SNew(SOverlay)//todo:add tooltip
				+ SOverlay::Slot()
				.Padding(FMargin(4.0f, 0.0f))
				[
					SNew(SSimpleGradient)
					.EndColor(this, &SColorPicker::handle_color_slider_end_color, channel)
					.StartColor(this, &SColorPicker::handle_color_slider_start_color, channel)
					.Orientation(Orient_Horizontal)
				]
				+ SOverlay::Slot()
				[
					SNew(SSlider)
					.IndentHandle(false)
					.Orientation(Orient_Vertical)
					.SliderBarColor(FLinearColor::Transparent)//don't hide slider bar
					.Style(&FAppStyle::get().get_widget_style<FSliderStyle>("ColorPicker.Slider"))
					.OnValueChanged(const_cast<SColorPicker*>(this), &SColorPicker::handle_color_spin_box_value_changed, channel)
				];
	}
	void SColorPicker::set_colors(const FLinearColor& in_color)
	{
		for (int32_t i = 0; i < m_target_f_colors.size(); ++i)
		{
			*m_target_f_colors[i] = in_color.to_fcolor(true);
		}

		for (int32_t i = 0; i < m_target_linear_colors.size(); ++i)
		{
			*m_target_linear_colors[i] = in_color;
		}
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
		const bool b_override_non_modal_creation = false;//todo:check modal

		std::shared_ptr<SWindow> window = nullptr;
		std::shared_ptr<SBorder> window_content = SNew(SBorder)
						.BorderImage(FAppStyle::get().get_brush("Brushes.Panel"))
						.Padding(FMargin(8.0f, 8.0f));

		bool b_need_to_add_window = true;//todo:add more parameter check

		//todo:add modal create

		if (!b_override_non_modal_creation)
		{
			//todo:non modal window

			//modal window
			window = SNew(SWindow)
					.ScreenPosition(adjusted_summon_location)//todo:add auto center
					.SizingRule(ESizingRule::AutoSized)
					.ClientSize(glm::vec2(600.0f, 400.0f))
					.Title("Color Picker")
					[
						window_content
					];
		}

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
		else
		{
			window_content->set_content(created_color_picker);

			if (b_need_to_add_window)
			{
				//todo:find the window of the parent widget
				//if (args.m_parent_widget)
				//{
				//	//
				//}
				window = Application::get().add_window(window);
			}

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