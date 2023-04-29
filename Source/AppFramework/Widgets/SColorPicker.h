#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"//SCompoundWidget depends on it

#include "Slate/Framework/SlateDelegates.h"

namespace DoDo {
	/*
	* struct for holding individual pointers to float values
	*/
	struct FColorChannels
	{
		FColorChannels()
		{
			m_red = m_green = m_blue = m_alpha = nullptr;
		}

		float* m_red;
		float* m_green;
		float* m_blue;
		float* m_alpha;
	};

	/*
	* enumerates color picker modesx
	*/
	enum class EColorPickerModes
	{
		Spectrum,
		Wheel
	};

	/*
	* enumerates color channels(do not reorder)
	*/
	enum class EColorPickerChannels
	{
		Red,
		Green,
		Blue,
		Alpha,
		Hue,
		Saturation,
		Value
	};
	/*
	* class for placing a color picker, if all you need is a standalone color picker
	* use the functions OpenColorPicker and DestroyPicker, since they hold a static 
	* instance of the color picker
	*/
	class SColorPicker : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SColorPicker)
		: _TargetColorAttribute(FLinearColor(ForceInit))
		, _TargetFColors()
		, _TargetLinearColors()
		, _UseAlpha(true)
		, _ParentWindow()
		{}
			/*the color that is being targeted as a TAttribute*/
			SLATE_ATTRIBUTE(FLinearColor, TargetColorAttribute)

			/*an array of color pointers this color picker targets*/
			SLATE_ATTRIBUTE(std::vector<FColor*>, TargetFColors)

			/*an array of linear color pointers this color picker targets*/
			SLATE_ATTRIBUTE(std::vector<FLinearColor*>, TargetLinearColors)

			/*whether the ability to pick the alpha value is enabled*/
			SLATE_ATTRIBUTE(bool, UseAlpha)

			/*the event called when the color is committed*/
			SLATE_EVENT(FOnLinearColorValueChanged, OnColorCommitted)

			/*a pointer to the parent window*/
			SLATE_ATTRIBUTE(std::shared_ptr<SWindow>, ParentWindow)
		SLATE_END_ARGS()
		/*destructor*/
		~SColorPicker();

		/*a default window size for the color picker which looks nice*/
		static const glm::vec2 default_window_size;
	public:
		/*
		* construct the widget
		* 
		* @param InArgs declaration from which to construct the widget
		*/
		void Construct(const FArguments& in_args);

		FLinearColor get_current_color() const
		{
			return m_current_color_hsv;
		}

		//callback for value changes in the color spectrum picker
		void handle_color_spectrum_value_changed(FLinearColor new_value);

		/*sets new color in either rgb or hsv*/
		bool set_new_target_color_hsv(const FLinearColor& new_value, bool b_force_update = false);
		bool set_new_target_color_rgb(const FLinearColor& new_value, bool b_force_update = false);

		bool apply_new_target_color(bool b_force_update = false);

		void update_color_pick();

		void update_color_pick_mouse_up();
	protected:
		void generate_inline_color_picker_content();

		//callback for getting the end color of a color slider
		FLinearColor handle_color_slider_end_color(EColorPickerChannels channel) const;

		//callback for getting the start color of a color slider
		FLinearColor handle_color_slider_start_color(EColorPickerChannels channel) const;

		//callback for when interactive user input begins
		void handle_interactive_change_begin();

		//callback for when interactive user input ends
		void handle_interactive_change_end();

		//callback for when interactive user input ends
		void handle_interactive_change_end(float new_value);

		//callback for value changes in a color spin box
		void handle_color_spin_box_value_changed(float new_value, EColorPickerChannels channel);

		/*
		* creates a color slider widget for the specified channel
		* 
		* @param Channel the color channel to create the widget for
		* @return the new slider
		*/
		std::shared_ptr<SWidget> make_color_slider(EColorPickerChannels channel) const;

		/*set all the colors to this new color*/
		void set_colors(const FLinearColor& in_color);

	private:
		/*invoked when a new value is selected on the color wheel*/
		FOnLinearColorValueChanged m_on_color_committed;

		/*the color that is being targeted as a TAttribute*/
		TAttribute<FLinearColor> m_target_color_attribute;

		/*an array of color pointers this color picker targets*/
		std::vector<FColor*> m_target_f_colors;

		/*an array of linear color pointers this color picker targets*/
		std::vector<FLinearColor*> m_target_linear_colors;

		/*the current color being picked in HSV*/
		FLinearColor m_current_color_hsv;

		/*the current color being picked in rgb*/
		FLinearColor m_current_color_rgb;

		/*the old color to be changed in hsv*/
		FLinearColor m_old_color;

		/*color end point to animate to*/
		FLinearColor m_color_end;

		/*color start point to animate from*/
		FLinearColor m_color_begin;

		/*time, used for color animation*/
		float m_current_time;

		/*tracks whether the user is moving a value spin box, the color wheel and the dropper*/
		bool m_b_is_interactive;
	};

	struct FColorPickerArgs
	{
		/*the parent for the new color picker window*/
		std::shared_ptr<SWindow> m_parent_widget;

		/*whether or not to enable the alpha slider*/
		bool b_user_alpha;

		/*whether to open the color picker as a menu window*/
		bool b_open_as_menu;

		/*an array of FColors to target*/
		const std::vector<FColor*>* m_color_array;

		/*an array of FLinearColors to target*/
		const std::vector<FLinearColor*>* m_linear_color_array;

		/*an array of FColorChannels to target.(deprecate now that wx is gone?)*/

		/*a delegate to be called when the color changes*/
		FOnLinearColorValueChanged m_on_color_committed;

		/*overrides the initial color set on the color picker*/
		FLinearColor m_initial_color_override;
	};

	/*open up the static color picker, destroying any previously existing one*/
	bool open_color_picker(const FColorPickerArgs& args);

	/*
	* destroy the current color picker, necessary if the values the color picker
	* currently targets become invalid
	*/
	void destroy_color_picker();
}